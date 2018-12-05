//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// -- ILLUMINIM LED DRIVER
// ---- type II - hue cycler
//
// ---- Authors: Callum Kirkwood, James Medd
//
// ---- takes rhythm data from https://illumin.im (via hardware.py), which is used to drive 9 DotStar strips wired in series
// ---- the colour of each strip changes in time with the rhythm, which plays once on each strip before moving down the series
// ---- two rhythms can play at the same time. The latest rhythm will loop forever - if a new one is received, the previous rhythm stops when it reaches the last strip
//
// ---- USE LINES 30-41 TO CUSTOMISE SETTINGS
// ------ flashTime sets the generic time delay of each beat/tap 
// ------ pauseBetweenBuoys sets the delay before moving on to the next strip
//
// ---- REMOTE RESET
// ------ Assumes the existence of an API endpoint that will trigger the Raspberry Pi to send a string command over serial
// ------ Change the value of resetCommand (line 41) to set the command phrase - it's currently looking out for the word "reset"
// ------ When the serial input matches the phrase stored in resetCommand, the reset function is triggered (lines 98-100)
//
// ---- OPTIONAL EXTRA - leave each lantern turned on as the rhythm moves on down the series
// ------ comment out lines 149 and 185 to leave all lanterns turned on
//
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <FastLED.h>

void(* resetFunc) (void) = 0; // set up reset function

// define the total number of LEDs, and the number of individual strips/lanterns
#define NUM_LEDS 184
#define BUOYS 4
#define MAX_VALUES 14 // DOESN'T SEEM TO LIKE ANY MORE THAN 14. For this to work, hardware.py needs to limit the number of beats sent to [0:13]

// define data and clock pins for the DotStars
#define DATA_PIN 11
#define CLOCK_PIN 13

int flashTime = 20; // represents the tap (i.e. the time the finger is pressed down)
int pauseBetweenBuoys = 700;
char resetCommand = "reset" // the phrase sent by the Raspberry Pi when a reset is required

uint8_t hue = 0;

// calculate the number of LEDs on each buoy
int PER_BUOY = NUM_LEDS / BUOYS;

// create a CRGBSet array
CRGBArray<NUM_LEDS> leds;

int intervals[MAX_VALUES]; // initialise an array to store rhythm values
int intervals2[MAX_VALUES]; // initialise a second array to store rhythm values

int numberOfBeats; // this will be used to determine how many beats we read into the intervals array later
int numberOfBeats2;

bool firstIsFull = false; // initialise a boolean that'll be used to check whether we already have a rhythm playing
int iterIndex;
int iterIndex2;

// set up the first buoy coordinates for the first stream
int first = 0;
int last = first + (PER_BUOY - 1);

// set up the first buoy coordinates for the second stream
int stream2_first = 0;
int stream2_last = first + (PER_BUOY - 1);

void setup() {
  Serial.begin(115200);
  FastLED.addLeds<DOTSTAR, DATA_PIN, CLOCK_PIN, BGR, DATA_RATE_KHZ(8)>(leds, NUM_LEDS); // initialise DotStars
  FastLED.setBrightness(255); // set max brightness
  FastLED.clear();
  FastLED.show();
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
    iterIndex = 0; // reset the assembly iterIndex
    if (firstIsFull) {
      numberOfBeats2 = 0;
      iterIndex2 = 0;
    }

    /*ITERATE OVER COMMA SEPARATED VALUES BEFORE THE NEWLINE (FIRST VALUE 'BEATS' GETS DROPPED ALTOGETHER BY SERIAL READ)*/
    while (Serial.read() != '\n') {
        if (Serial.read == resetCommand) { // reset the Teensy if requested by the Pi
          resetFunc();
        }
      if (!firstIsFull) { // if the first array is empty...
        intervals[iterIndex] = Serial.parseInt();//read in a beat interval
        iterIndex += 1 ;
      } else { // else if the first array is full...
        intervals2[iterIndex2] = Serial.parseInt();//read in a beat interval
        iterIndex2 += 1 ;
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

  doubleHueCycle(); // trigger main function

}



void doubleHueCycle() {
  // set starting colours
  //int hue1 = startingColour1;
  //int hue2 = startingColour2;

  if (intervals[0] != 0) { // if a rhythm is received...
    for (int j = 0; j < numberOfBeats + 2; j++) { // for each available tap...
      int beat = intervals[j];
      leds(first, last) =  CHSV(hue, 255, 255); // set the buoy's colour
      FastLED.show();
      FastLED.delay(flashTime);
      hue += 80; // update the hue variable ready for the next colour change
      delay(beat); // delay, using the latest rhythm value
      if (j > numberOfBeats) { // add a final flash
        FastLED.delay(flashTime);
      }
    }
    delay(pauseBetweenBuoys); // wait before moving on
    leds(first, last) = CRGB::Black; // comment out this line to leave each buoy turned on as the rhythm moves down
    FastLED.show();

    // jump to the next buoy
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
    if (first > (stream2_first + (PER_BUOY * 2)) || first < (stream2_first - PER_BUOY) || intervals[0] == 0) { // if the previous rhythm is at least 2 strips away...
      for (int j = 0; j < numberOfBeats2 + 2; j++) { // start flashing a new rhythm
        int beat = intervals2[j];
        leds(stream2_first, stream2_last) =  CHSV(hue * 2, 255, 255); // make sure the starting hue is different to the first rhythm
        FastLED.show();
        FastLED.delay(flashTime); 
        hue += 80; // update the hue variable ready for the next colour change
        delay(beat); // delay, using the latest rhythm value
        if (j > numberOfBeats2) { // add a final flash
          FastLED.delay(flashTime);
        }
      }
      delay(pauseBetweenBuoys); // wait before moving on
      leds(stream2_first, stream2_last) = CRGB::Black; // comment out this line to leave each buoy turned on as the rhythm moves down
      FastLED.show();

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
