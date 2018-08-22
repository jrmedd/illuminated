import microbit
import neopixel

NP = neopixel.NeoPixel(microbit.pin2, 1)  # one neopixel on pin 2

def flash(duration):
    for i in range(5):
        brightness = int((255/4)*((i+1) % 5))
        NP[0] = ((brightness),)*3
        NP.show()
        microbit.sleep(duration/5)
