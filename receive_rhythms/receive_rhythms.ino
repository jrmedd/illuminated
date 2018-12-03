#define MAX_VALUES 20

int intervals[MAX_VALUES]; //initialise an array
int numberOfBeats; // this will be used to determine how many beats we read into the intervals array later

void setup() {
  Serial.begin(115200);

}

void loop() {
  /*WHEN SERIAL EVENT COMES IN BELOW*/
  if (Serial.available() > 0) {
    /*CLEAR THE ARRAY (POPULATE WITH ZEROES*/
    for (int i = 0; i < MAX_VALUES; i++) {
      intervals[i] = 0; //pack the array with 20 zeroes
    }
    numberOfBeats = 0;//reset the beat count
    int index = 0; //reset the assembly index
    /*ITERATE OVER COMMA SEPARATED VALUES BEFORE THE NEWLINE (FIRST VALUE 'BEATS' GETS DROPPED ALTOGETHER BY SERIAL READ)*/
    while (Serial.read() != '\n') {
         intervals[index] = Serial.parseInt();//read in a beat interval
         Serial.println(intervals[index]);
         index +=1 ;
    }
    /*DETERMINE THE NUMBER OF BEATS BELOW*/
    for (int i = 0; i < MAX_VALUES; i++) {
      int currentNumber = intervals[i];
      if (currentNumber == 0) {
        numberOfBeats = i;
        break;
      }
    }
    /*NOW THE BEATS ARE IN INTERVALS ARRAY AND WE KNOW HOW MANY ARE IN THERE, DO THE STUFF BELOW*/
  }
}
