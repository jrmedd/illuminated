import requests
import time
import serial

RHYTHM_URL = "http://127.0.0.1:5000/rhythms"

MB = serial.Serial('/dev/tty.usbmodem1412', baudrate=115200)

while True:
    oldest_rhythm = requests.get(RHYTHM_URL).json().get('to_illuminate')
    if oldest_rhythm:
        beats = oldest_rhythm.get('_taps')
        flash = ('flash(%d)\r' % (beats[0]*0.25))
        MB.write(flash.encode())
        time.sleep(beats[0]*0.001)
        for beat in range(len(beats)):
            print('beat')
            flash = 'flash(%d)\r' % (beats[(beat+1)%len(beats)]*0.25)
            MB.write(flash.encode())
            time.sleep(beats[(beat+1)%len(beats)]*0.001)
    time.sleep(5)
