import requests
import time
import serial

import os

RHYTHM_URL = "https://illumin.im/rhythms"
#RHYTHM_URL = "http://127.0.0.1:5000/rhythms"

TEENSY = serial.Serial(port='/dev/ttyACM0', baudrate=115200)

API_KEY = os.environ.get('API_KEY')

API_HEADER = {'X-Api-Key':API_KEY}

while True:
    try:
        oldest_rhythm = requests.get(RHYTHM_URL, headers=API_HEADER).json().get('to_illuminate') #get the JSON
    except:
        print("Unable to access")
    if oldest_rhythm:
        beats = oldest_rhythm.get('_taps')[0:19] #always strip it to 20 beats
        #comma-separate the beats as ints in a string, the 'BEATS' bit gets chopped off when first reading on the Arduino
        beats_string = "BEATS,%s\n" % (','.join([str(int(beat)) for beat in beats])) 
        TEENSY.write(beats_string.encode())
        update = requests.post(RHYTHM_URL, headers=API_HEADER ,data={'_id':oldest_rhythm.get('_id'), 'illuminated':True})
        time.sleep((oldest_rhythm.get('duration')/1000)*9)
    else:
        time.sleep(5)
