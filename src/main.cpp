#include <Arduino.h>
#include <Adafruit_NeoPixel.h>

#define LED_PIN 6
#define LED_COUNT 24
#define BRIGHTNESS 10

#define BTN_MODE_PIN 7
#define DEBOUNCE_DELAY 50
#define PATTERN_NUMBER 6

#define SERIAL_BAUD 9600


Adafruit_NeoPixel strip(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800);
uint32_t OFF = Adafruit_NeoPixel::Color(0, 0, 0);
uint32_t DARKTURQUOISE = Adafruit_NeoPixel::Color(0, 206, 209);
uint32_t RED = Adafruit_NeoPixel::Color(235, 33, 46);
uint32_t GREEN = Adafruit_NeoPixel::Color(47, 249, 36);
uint32_t BLUE = Adafruit_NeoPixel::Color(46, 103, 248);

uint32_t lastUpdate = 0;
uint8_t patternInterval = 20;
uint8_t intervals[] = {20, 20, 50, 100, 150, 20}; // speed for each pattern

uint32_t Wheel(uint8_t WheelPos) {
  WheelPos = 255 - WheelPos;
  uint16_t trippleWheelPos = WheelPos * 3;
  if (WheelPos < 85) {
    return Adafruit_NeoPixel::Color(255 - trippleWheelPos, 0, trippleWheelPos);
  }
  if (WheelPos < 170) {
    WheelPos -= 85;
    return Adafruit_NeoPixel::Color(0, trippleWheelPos, 255 - trippleWheelPos);
  }
  WheelPos -= 170;
  return Adafruit_NeoPixel::Color(trippleWheelPos, 255 - trippleWheelPos, 0);
}


void rainbow() {
  static uint8_t j = 0;
  for (uint16_t i = 0; i < strip.numPixels(); i++) {
    strip.setPixelColor(i, Wheel((i + j) & 255));
  }
  strip.show();
  j++;
  lastUpdate = millis();
}

void wipe() {
  for (uint16_t i = 0; i < strip.numPixels(); i++) {
    strip.setPixelColor(i, OFF);
  }
}

void colorWipe(uint32_t c) {
  static uint16_t i = 0;
  strip.setPixelColor(i++, c);
  strip.show();
  if (i >= strip.numPixels()) {
    i = 0;
    wipe();
  }
  lastUpdate = millis();
}

void updatePattern(uint8_t pat) {
  switch (pat) {
    case 0:
      rainbow();
      break;
    case 1:
      colorWipe(DARKTURQUOISE);
      break;
    case 2:
      colorWipe(GREEN);
      break;
    case 3:
      colorWipe(RED);
      break;
    case 4:
      colorWipe(BLUE);
      break;
    case 5:
      colorWipe(OFF);
      break;
    default:
      rainbow();
  }
}


void setup() {
  strip.begin();
  strip.setBrightness(BRIGHTNESS);
  strip.show();

  pinMode(BTN_MODE_PIN, INPUT);

  // initialize serial communication:
  Serial.begin(SERIAL_BAUD);
}


void loop() {
  static uint8_t pattern = 0, lastReading;
  uint8_t reading = digitalRead(BTN_MODE_PIN);
  if (lastReading == HIGH && reading == LOW) {
    pattern = (pattern + 1) % PATTERN_NUMBER;
    patternInterval = intervals[pattern];
    wipe();
    delay(DEBOUNCE_DELAY);
  }
  lastReading = reading;
  if (millis() - lastUpdate > patternInterval) updatePattern(pattern);
}



