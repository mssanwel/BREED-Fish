from sense_hat import SenseHat
import RPi.GPIO as GPIO
from time import sleep
GPIO.setmode(GPIO.BOARD)

def SetAngle(angle1,angle2):
    duty1=angle1/18+2
    duty2=angle2/18+2
    GPIO.output(servoPin1, True)
    GPIO.output(servoPin2, True)
    pwm1.ChangeDutyCycle(duty1)
    pwm2.ChangeDutyCycle(duty2)
    sleep(1)
    GPIO.output(servoPin1, False)
    GPIO.output(servoPin2, False)
    pwm1.ChangeDutyCycle(0)
    pwm2.ChangeDutyCycle(0)


sense = SenseHat()
servoPin1=26
servoPin2=16
GPIO.setup(servoPin1, GPIO.OUT)
GPIO.setup(servoPin2, GPIO.OUT)
pwm1=GPIO.PWM(servoPin1, 50)
pwm2=GPIO.PWM(servoPin2, 50)
pwm1.start(0)
pwm2.start(0)

while True:
	acceleration = sense.get_accelerometer_raw()
	x = acceleration['x']
	y = acceleration['y']
	z = acceleration['z']

	#x=round(x, 0)
	#y=round(y, 0)
	#z=round(z, 0)
	#sleep(1)
	print("x={0}, y={1}, z={2}".format(x, y, z))
	
	#if x>=0
	SetAngle(x*90,180-(x*90))
	#	print("Turning")
		#sleep(1)
	#else x==0:
	#	SetAngle(90, 90)
		#sleep(1)
	
