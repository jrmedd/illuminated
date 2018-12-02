//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// -- ILLUMINIM LED DRIVER
//
// ---- Authors: Callum Kirkwood, James Medd
//
// ---- takes rhythm data from https://illumin.im (via hardware.py), which is used to drive 9 DotStar strips wired in series
// ---- for each rhythm received, a dot wipes across each strip from left to right, leaving a fading tail. The rhythm plays once per strip and moves down the series.
// ---- two rhythms can play at the same time, denoted by different colour gradients
// ---- the latest rhythm will loop forever - if a new one is received, the previous rhythm stops when it reaches the last strip
//
// ---- USE LINES 23-37 TO CUSTOMISE SETTINGS
// ------ startingColour1 and startingColour2 take a hue value, as the starting point for a gradient across all strips
// ------ speedAdjust is used to calibrate the incoming rhythm data for the amount of pixels on each strip. 10 is a good number for 30 pixels (go higher if using more)
// ------ fadeAmount sets the size of the fading tail - 20 is a good number if using 30 pixels (go lower if using more)
// ------ flashTime sets the time it takes for the dot to travel from left to right on each beat - 10 is a good number if using 30 pixels (go lower if using more pixels)
// ------ pauseBetweenBuoys sets the delay before moving on to the next strip
//
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <FastLED.h>

// define the total number of LEDs, and the number of individual strips/lanterns
#define NUM_LEDS 30
#define BUOYS 5
#define MAX_VALUES 14 // DOESN'T SEEM TO LIKE ANY MORE THAN 14. For this to work, hardware.py needs to limit the number of beats sent to [0:13]

// define data and clock pins for the DotStars
#define DATA_PIN 9
#define CLOCK_PIN 10

int startingColour1 = 128; // aqua
int startingColour2 = 192; // purple
int speedAdjust = 2; // higher number for more pixels, i.e 10 if using 30 pixels
int fadeAmount = 80; // lower number for more pixels, i.e. 20 if using 30 pixels
int flashTime = 20; // lower number for more pixels, i.e. 10 if using 30 pixels
int pauseBetweenBuoys = 400;


// calculate the number of LEDs on each buoy
int PER_BUOY = NUM_LEDS / BUOYS;

// create a CRGBSet array
CRGBArray<NUM_LEDS> leds;

int intervals[MAX_VALUES]; // initialise an array to store rhythm values
int intervals2[MAX_VALUES]; // initialise a second array to store rhythm values

int numberOfBeats; // this will be used to determine how many beats we read into the intervals array later
int numberOfBeats2;

bool firstIsFull = false; // initialise a boolean that'll be used to check whether we already have a rhythm playing
int index;
int index2;

// set up the first buoy coordinates for the first stream
int first = 0;
int last = first + (PER_BUOY - 1);

// set up the first buoy coordinates for the second stream
int stream2_first = 0;
int stream2_last = first + (PER_BUOY - 1);

void setup() {
  Serial.begin(115200);

  FastLED.addLeds<APA102, DATA_PIN, CLOCK_PIN, BGR>(leds, NUM_LEDS); // initialise DotStars
  FastLED.setBrightness(255); // set max brightness
}

void loop() {
  /*WHEN SERIAL EVENT COMES IN BELOW*/
  if (Serial.available() > 0) {
    /*CLEAR THE ARRAY (POPULATE WITH ZEROES*/
    for (int i = 0; i < MAX_VALUES; i++) {
      if (!firstIsFull) {
        intervals[i] = 0; // pack the array with 20 zeroes
      } else {
        intervals2[i] = 0;
      }
    }

    numberOfBeats = 0;// reset the beat count
    index = 0; // reset the assembly index
    if (firstIsFull) {
      numberOfBeats2 = 0;
      index2 = 0;
    }

    /*ITERATE OVER COMMA SEPARATED VALUES BEFORE THE NEWLINE (FIRST VALUE 'BEATS' GETS DROPPED ALTOGETHER BY SERIAL READ)*/
    while (Serial.read() != '\n') {
      if (!firstIsFull) { // if the first array is empty...
        intervals[index] = Serial.parseInt();//read in a beat interval
        index += 1 ;
      } else { // else if the first array is full...
        intervals2[index2] = Serial.parseInt();//read in a beat interval
        index2 += 1 ;
      }
    }

    // toggle boolean
    firstIsFull = !firstIsFull;


    // set starting point
    if (firstIsFull) {
      first = 0;
      last = first + (PER_BUOY - 1);
    }

    /*DETERMINE THE NUMBER OF BEATS AVAILABLE*/
    for (int i = 0; i < MAX_VALUES; i++) {
      int currentNumber = intervals[i];
      if (currentNumber == 0) {
        numberOfBeats = i;
        break;
      }
    }
    for (int i = 0; i < MAX_VALUES; i++) {
      int currentNumber = intervals2[i];
      if (currentNumber == 0) {
        numberOfBeats2 = i;
        break;
      }
    }
  }

  doubleFadeTail(); // trigger main function

}



void doubleFadeTail() {
  // set starting colours
  int hue1 = startingColour1;
  int hue2 = startingColour2;

  if (intervals[0] != 0) { // if a rhythm is received...
    for (int j = 0; j < numberOfBeats + 1; j++) { // for each available tap...
      for (int i = first; i < last + 1; i++) { // for each LED on the buoy...
        fadeToBlackBy(leds, NUM_LEDS, fadeAmount); // fade all LEDS
        leds[i] = CHSV(hue1 + i, 255, 255); // turn on the next leading LED
        FastLED.show();
        FastLED.delay(flashTime); //
        fadeToBlackBy(leds, NUM_LEDS, fadeAmount); // fade all LEDS
      }
      delay(intervals[j] / speedAdjust); // delay, using the latest rhythm value
      if (j > numberOfBeats) { // add a final flash
        FastLED.delay(flashTime);
      }
    }
    delay(pauseBetweenBuoys); // pause before moving on to the next strip

    // jump to the next strip
    first += PER_BUOY;
    last += PER_BUOY;
  }

  // if a new rhythm is received, wait until this one reaches the last strip and then pack the array with zeroes to stop it looping
  if (intervals2[0] != 0 && first >= (NUM_LEDS - 1)) {
    for (int i = 0; i < MAX_VALUES; i++) {
      intervals[i] = 0;
    }
  }

  // if this rhythm reaches the end (and a new one has not been received yet), loop back to the start
  if (first >= (NUM_LEDS - 1)) {
    first = 0;
    last = PER_BUOY - 1;
  }

  // if a second rhythm is available, repeat as above - in a second colour
  if (intervals2[0] != 0) {
    if (first > (stream2_first + (PER_BUOY * 2)) || first < (stream2_first - (PER_BUOY)) || !leds[stream2_first]) { // if the previous rhythm is at least 2 strips away...
      for (int j = 0; j < numberOfBeats2 + 1; j++) { // for each available tap...
        for (int i = stream2_first; i < stream2_last + 1; i++) { // for each LED on the buoy...
          fadeToBlackBy(leds, NUM_LEDS, fadeAmount); // fade all LEDS
          leds[i] = CHSV(hue2 + i, 255, 255); // turn on the next leading LED
          FastLED.show();
          FastLED.delay(flashTime);
          fadeToBlackBy(leds, NUM_LEDS, fadeAmount); // fade all LEDS
        }
        delay(intervals2[j] / speedAdjust); // delay, using the latest rhythm value
        if (j > numberOfBeats2) { // add the final flash
          FastLED.delay(flashTime);
        }
      }
      delay(pauseBetweenBuoys); // pause before moving on to the next strip

      // jump to the next buoy
      stream2_first += PER_BUOY;
      stream2_last += PER_BUOY;

      // if a new rhythm is received, wait until this one reaches the last strip and then pack the array with zeroes to stop it looping
      if (intervals[0] != 0 && stream2_first >= (NUM_LEDS - 1)) {
        for (int i = 0; i < MAX_VALUES; i++) {
          intervals2[i] = 0;
        }
      }

      // if this rhythm reaches the end (and a new one has not been received yet), loop back to the start
      if (stream2_first >= (NUM_LEDS - 1)) {
        stream2_first = 0;
        stream2_last = PER_BUOY - 1;
      }

    }
  }
}

