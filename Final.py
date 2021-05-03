
#!/usr/bin/env python3

from sense_hat import SenseHat
import RPi.GPIO as GPIO
from time import sleep
import serial
GPIO.setmode(GPIO.BOARD)
GPIO.cleanup()

def SetAngle(angle1,angle2):
    print("Angle1", angle1)
    print("Angle2", angle2)
    duty1=angle1/18+2
    duty2=angle2/18+2
    GPIO.output(servoPin1, True)
    GPIO.output(servoPin2, True)
    pwm1.ChangeDutyCycle(duty1)
    pwm2.ChangeDutyCycle(duty2)
    sleep(0.2)
    GPIO.output(servoPin1, False)
    GPIO.output(servoPin2, False)
    pwm1.ChangeDutyCycle(0)
    pwm2.ChangeDutyCycle(0)

def write_to_servo(numR, numU):
  if (numU>4 and numR==4):
    print("Going Up")
    x=1
    SetAngle(int(50-x*50), int(50-x*50))
  elif (numU<4 and numR==4):
    print("Going Down")
    x=-1
    SetAngle(int(50-x*50), int(50-x*50))
  elif (numR>4 and numU==4):
    print("Rolling Right")
    x=1
    SetAngle(int(50-x*50), int(50+x*50))
  elif (numR<4 and numU==4):
    print("Rolling Left")
    x=-1
    SetAngle(int(50-x*50), int(50+x*50))
  elif (numR==4 and numU==4):
    print("Nothingt")
    x=0
    SetAngle(int(50-x*50), int(50+x*50))
    
    

def main(args):
    return 0

if __name__ == '__main__':
    ser = serial.Serial('/dev/ttyACM0', 9600)
    ser.flush
    #SetAngle(int(50-x*50), int(50+x*50))
    sense = SenseHat()
    servoPin1=26
    servoPin2=24
    GPIO.setup(servoPin1, GPIO.OUT)
    GPIO.setup(servoPin2, GPIO.OUT)
    pwm1=GPIO.PWM(servoPin1, 50)
    pwm2=GPIO.PWM(servoPin2, 50)
    pwm1.start(0)
    pwm2.start(0)

	
    #sleep(3)
    cmd=None
    while True:
        if ser.in_waiting > 0:
            line = ser.readline().rstrip()
            parse = line.split(': ')
            if(len(parse) > 1 and (parse[0][0] == "C" or parse[0][0] == "R")):
                print(parse[1])
		cmd=parse[1]
		numR=int(cmd[1])
		numU=int(cmd[3])
		print("R Command", numR)
		print("U Command", numU)
		write_to_servo(numR, numU)
	
		#print(parse[1][1])
		#print(parse[1][2])
		#print(parse[1][3])
		#print(parse[1][4])
	acceleration = sense.get_accelerometer_raw()
	x = acceleration['x']
	y = acceleration['y']
	z = acceleration['z']

	#x=round(x, 0)
	#y=round(y, 0)
	#z=round(z, 0)
	#sleep(1)
	#print("x={0}, y={1}, z={2}".format(x, y, z))
	
	#SetAngle(int(50-x*50), int(50+x*50))
	






pwm.stop
GPIO.cleanup()
