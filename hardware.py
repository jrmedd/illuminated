import requests
import time
import serial

RHYTHM_URL = "http://127.0.0.1:5000/rhythms"

MB = serial.Serial('/dev/tty.usbmodem1412', baudrate=115200)

while True:
    oldest_rhythm = requests.get(RHYTHM_URL).json().get('to_illuminate')
    beats = oldest_rhythm.get('_taps')
    for beat in beats:
        print('beat')
        flash = 'flash(%d)\r' % (beat*0.25)
        MB.write(flash.encode())
        time.sleep(beat*0.001)
