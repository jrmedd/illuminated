import microbit
import neopixel
import radio

radio.config(power=7, channel=33)
radio.on()

FLASH_TIME = 100

previous_receive = []
NP = neopixel.NeoPixel(microbit.pin2, 1)  # one neopixel on pin 2

def flash(duration):
    for i in range(5):  #iterate over 5 brightness levels (including off)
        brightness = int((255/4)*((i+1) % 5))  #split max brightness, finish at 0
        NP[0] = ((brightness),)*3  #fill tuple with the same level (white)
        NP.show()
        microbit.sleep(duration/5)  #wait for the total duration

while True:
    received = radio.receive()
    if received:
        if received != previous_receive:  #master micro:bit sends 3 of the same message to reduce loss
            beats = [float(beat) for beat in received.split()]  #split the string into a list
            flash(FLASH_TIME) # first beat
            microbit.sleep(beats[0])
            for beat in range(len(beats)): # remaining beats
                this_beat = beats[(beat+1)%len(beats)]
                flash(FLASH_TIME)
                microbit.sleep(this_beat)
            previous_receive = received
