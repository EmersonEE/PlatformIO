#include "crgb.h"
#include "fl/fill.h"
#include <Arduino.h>
#include <FastLED.h>

#define DATA_PIN 3
#define NUM_LEDS 64

CRGB leds[NUM_LEDS];

void setup() { FastLED.addLeds<WS2812B, DATA_PIN, GRB>(leds, NUM_LEDS); }

void loop() {

  fl::fill_solid(leds, NUM_LEDS, CRGB::Yellow);
  FastLED.show();
  delay(1500);
  fill_solid(leds, NUM_LEDS, CRGB::Blue);
  FastLED.show();
  delay(1500);
  fill_solid(leds, NUM_LEDS, CRGB::Red);
  FastLED.show();
  delay(1500);
}
