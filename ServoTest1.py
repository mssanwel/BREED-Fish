#!/usr/bin/env python3

import RPi.GPIO as GPIO
from time import sleep
GPIO.setmode(GPIO.BOARD)

servoPin = 24

GPIO.setup(servoPin, GPIO.OUT)
pwm = GPIO.PWM(servoPin, 50)

pwm.start(0)


def setAngle(angle):
	duty = angle/18+2
	GPIO.output(servoPin, True)
	pwm.ChangeDutyCycle(duty)
	sleep(1)
	GPIO.output(servoPin, False)
	pwm.ChangeDutyCycle(0)
while (True):
	setAngle(0)
	sleep(1)
	setAngle(90)
	sleep(1)
	setAngle(180)
	sleep(1)

pwm.stop
GPIO.cleanup()
