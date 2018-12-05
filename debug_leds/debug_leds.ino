#include <FastLED.h>

#define NUM_LEDS 184
// define data and clock pins for the DotStars
#define DATA_PIN 11
#define CLOCK_PIN 13

CRGB leds[NUM_LEDS];

void setup() {
    FastLED.addLeds<DOTSTAR, DATA_PIN, CLOCK_PIN, BGR, DATA_RATE_KHZ(8)>(leds, NUM_LEDS); // initialise DotStars
    FastLED.setBrightness(255);
    FastLED.clear();
    FastLED.show();
    for (int i = 0; i < NUM_LEDS; i ++) {
        leds[i] = CRGB::Red;
        FastLED.show();
        delay(5);
    }
}
void loop(){
}
