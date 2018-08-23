import requests
import time
import serial

import os

RHYTHM_URL = "https://illumin.im/rhythms"

MB = serial.Serial('/dev/tty.usbmodem1412', baudrate=115200)

API_KEY = os.environ.get('API_KEY')

API_HEADER = {'X-Api-Key':API_KEY}

def flash(dur):
    flash_command = ('flash(%d)\r' % (dur*0.5))
    MB.write(flash_command.encode())
    time.sleep(dur*0.001)

while True:
    oldest_rhythm = requests.get(RHYTHM_URL, headers=API_HEADER).json().get('to_illuminate')
    if oldest_rhythm:
        beats = oldest_rhythm.get('_taps')
        flash(beats[0])
        for beat in range(len(beats)):
            print('beat')
            flash (beats[(beat+1)%len(beats)])
        update = requests.post(RHYTHM_URL, headers=API_HEADER ,data={'_id':oldest_rhythm.get('_id'), 'illuminated':True})
    time.sleep(6)
