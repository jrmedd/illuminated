import microbit
import radio

radio.config(power=7, channel=33)
radio.on()

def send_rhythm(beat_array):
    beat_array_str = ' '.join(str(beat) for beat in beat_array) #convert array of beats to string
    for i in range(3):  #send three times to prevent loss
        radio.send(beat_array_str)
