import requests
import time
import serial

RHYTHM_URL = "http://127.0.0.1:5000/rhythms"

MB = serial.Serial('/dev/tty.usbmodem1412', baudrate=115200)

def flash(dur):
    flash_command = ('flash(%d)\r' % (dur*0.5))
    MB.write(flash_command.encode())
    time.sleep(dur*0.001)

while True:
    oldest_rhythm = requests.get(RHYTHM_URL).json().get('to_illuminate')
    if oldest_rhythm:
        beats = oldest_rhythm.get('_taps')
        flash(beats[0])
        for beat in range(len(beats)):
            print('beat')
            flash (beats[(beat+1)%len(beats)])
        update = requests.post(RHYTHM_URL, data={'_id':oldest_rhythm.get('_id'), 'illuminated':True})
    time.sleep(6)
