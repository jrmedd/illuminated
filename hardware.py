import requests
import time
import serial
import logging

logging.basicConfig(filename='errors.log',level=logging.DEBUG)

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
        beats = oldest_rhythm.get('_taps')[0:13] #always strip it to 15 beats
        #comma-separate the beats as ints in a string, the 'BEATS' bit gets chopped off when first reading on the Arduino
        beats_string = "BEATS,%s\n" % (','.join([str(int(beat)) for beat in beats])) 
        print(beats_string)
        TEENSY.write(beats_string.encode())
        update = requests.post(RHYTHM_URL, headers=API_HEADER ,data={'_id':oldest_rhythm.get('_id'), 'illuminated':True})
        time.sleep((oldest_rhythm.get('duration')/1000)*9)
    else:
        beats_string = "BEATS,200,200,200,200\n"
        print(beats_string)
        TEENSY.write(beats_string.encode())
        time.sleep(3.2)
