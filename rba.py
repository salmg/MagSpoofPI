import RPi.GPIO as GPIO
from time import sleep
pin = 7
GPIO.setmode(GPIO.BOARD)
GPIO.setwarnings(False)
GPIO.setup(pin, GPIO.OUT)
GPIO.output(pin, GPIO.HIGH) #Send signal to Attiny simulating a pressed button.
sleep(.5)
GPIO.output(pin, GPIO.LOW) 
sleep(.5)
GPIO.cleanup()
