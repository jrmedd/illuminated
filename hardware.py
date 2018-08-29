import requests
import time
import serial

import os

RHYTHM_URL = "https://illumin.im/rhythms"

MB = serial.Serial('/dev/tty.usbmodem1412', baudrate=115200)

API_KEY = os.environ.get('API_KEY')

API_HEADER = {'X-Api-Key':API_KEY}

def send_rhythm(beat_array):
    to_transmit = 'send_rhythm(%a)\r' % (beat_array)
    MB.write(to_transmit.encode())
    print("Sent %s" % (MB.readline()))

while True:
    oldest_rhythm = requests.get(RHYTHM_URL, headers=API_HEADER).json().get('to_illuminate')
    if oldest_rhythm:
        beats = oldest_rhythm.get('_taps')
        send_rhythm(beats)
        update = requests.post(RHYTHM_URL, headers=API_HEADER ,data={'_id':oldest_rhythm.get('_id'), 'illuminated':True})
    time.sleep(6)
