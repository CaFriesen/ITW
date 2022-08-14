#include "FastLED.h"
#include "Easing.h"

// 5 beams elk 5 ledstrips 180 leds per strip 900 per beam

#if defined(FASTLED_VERSION) && (FASTLED_VERSION < 3001000)
#warning "Requires FastLED 3.1 or later; check github for latest code."
#endif

#define DATA_PIN            3
#define LED_TYPE            WS2812B
#define COLOR_ORDER         GRB
#define NUM_LEDS            60
#define NUM_STRIPS_PER_BEAM 5
#define NUM_BEAMS           5
#define BRIGHTNESS          96
#define buttonPin           A0
#define threshold           40
const int ledPin =  13;      // the number of the LED pin

CRGB ledsRGB[NUM_LEDS];
CHSV ledsHSV[NUM_LEDS];
int trig[NUM_LEDS + 1];
int buttonState = 0;
bool buttonFlag = false;
int FRAMES_PER_SECOND = 60;
long long timestamp;

EasingFunc<Ease::QuadInOut> e;
float start;

void sinelon()
{

  for (int i = 0; i < NUM_LEDS; i++) {
    if (trig[i] == 1) {
      ledsHSV[i].v = 200;
      ledsHSV[i].h += 40;
      ledsHSV[i].s = 0;
    }
    ledsHSV[i].v = max(ledsHSV[i].v - random(2)*20, 0);
    ledsHSV[i].s = min(ledsHSV[i].s + 50, 255);
  }
}

void shiftToRight(int a[], int n) {
  int temp = a[n];

  for (int i = n; i > 0; i--) {
    a[i] = a[i - 1];
  }
  a[0] = 0;
}

void hsv2rgb() {
  for (int i = 0; i < NUM_LEDS; i++) {
    ledsRGB[i] = ledsHSV[i];
  }
}

int trigIntensity(uint16_t inputSensor){
  Serial.println(inputSensor);
  if(inputSensor > threshold)
  {
    static int maxInput;
    static int counter;
    if(maxInput < inputSensor)
    {
      maxInput = inputSensor;
      counter = 0;
      return inputSensor;
    }
    else
    {
      counter++;
      if (counter >= 10)
      {
        maxInput = 0;
        counter = 0;
      }
      return 0;
    }
  }
  else
  {
    return 0;
  }
}

void setup() {
  Serial.begin(115200);
  delay(3000); // 3 second delay for recovery

  FastLED.addLeds<LED_TYPE, DATA_PIN, COLOR_ORDER>(ledsRGB, NUM_LEDS).setCorrection(TypicalLEDStrip);
  FastLED.setBrightness(BRIGHTNESS);

  Serial.begin(9600);
  timestamp = 0;

  // initialize the LED pin as an output:
  pinMode(ledPin, OUTPUT);
}

void loop() {
  // read the state of the pushbutton value:
  uint16_t intensity = trigIntensity(analogRead(buttonPin));
  if(intensity > 0 && timestamp+80 < millis())
  {
    FRAMES_PER_SECOND = intensity;
    buttonState = true;
    timestamp = millis();
  }
  else
  {
    buttonState = false;
  }

// check if the pushbutton is pressed. If it is, the buttonState is HIGH:
  if (buttonState == HIGH && buttonFlag == false) {
    trig[0] = 1;
    buttonFlag = true;
    digitalWrite(ledPin, HIGH);
  } else if(buttonState = LOW && buttonFlag == true){
    buttonFlag = false;
    digitalWrite(ledPin, LOW);
  }
  
  shiftToRight(trig, 61);
  sinelon();
  hsv2rgb();
  FastLED.show();
  //FastLED.delay(40 / FRAMES_PER_SECOND);

}
