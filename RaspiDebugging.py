
#!/usr/bin/env python3

from sense_hat import SenseHat
import RPi.GPIO as GPIO
from time import sleep
import serial
GPIO.setmode(GPIO.BOARD)

def SetAngle(angle1,angle2):
    print("Angle1", angle1)
    print("Angle2", angle2)
    duty1=angle1/18+2
    duty2=angle2/18+2
    GPIO.output(servoPin1, True)
    GPIO.output(servoPin2, True)
    pwm1.ChangeDutyCycle(duty1)
    pwm2.ChangeDutyCycle(duty2)
    sleep(0.1)
    GPIO.output(servoPin1, False)
    GPIO.output(servoPin2, False)
    pwm1.ChangeDutyCycle(0)
    pwm2.ChangeDutyCycle(0)

def maps(val, ra1, ra2, rb1, rb2):
    return ((val/ra2)*abs(rb1-rb2))-rb2   

def write_to_servo(numR, numU):
    min_range=0
    mid_range=4
    max_range=9
    min_angle=45
    mid_angle=90
    max_angle=135
    if ((numR > 5 or numR < 4) and (numU > 5 or numU < 4)):  #if both R and U signals are not 4-5, both signals written to servo
	if (numU >= 0 and numU < 4):
	  s1 = maps(numU, min_range, mid_range - 1, min_angle, mid_angle) #move down
	  s2 = maps(numU, min_range, mid_range - 1, min_angle, mid_angle)
	else:
	  s1 = maps(numU, mid_range + 2, max_range, mid_angle, max_angle) #move up
	  s2 = maps(numU, mid_range + 2, max_range, mid_angle, max_angle)
	#print out coordinates/angles of servo motors
	print("\n")
	print("output to servo1: ")
	print(s1)
	print("\n")
	print("output to servo2: ")
	print(s2)
	print("\n")
	#motors turn according to signal and go back to original position
	#servo1.write(s1);
	#servo2.write(s2);
	SetAngle(s1, s2)
	sleep(0.3)
	if (numR >= 0 and numR < 4):
	  s1 = maps(numR, min_range, mid_range - 1, min_angle, mid_angle) //left
	  s2 = maps(numR, min_range, mid_range - 1, max_angle, mid_angle)
	else:
	  s1 = maps(numR, mid_range + 2, max_range, mid_angle, max_angle)
	  s2 = maps(numR, mid_range + 2, max_range, mid_angle, min_angle)
	#print out coordinates/angles of servo motors
	print("\n")
	print("output to servo1: ")
	print(s1)
	print("\n")
	print("output to servo2: ")
	print(s2)
	print("\n")
	#motors turn according to signal and go back to original position
	#servo1.write(s1);
	#servo2.write(s2);
	SetAngle(s1, s2)
    

    elif ((numR > 5 or numR < 4) and numU <= 5 and numU >= 4): 
	if (numR >= 0 and numR < 4):
	    s1 = maps(numR, min_range, mid_range - 1, min_angle, mid_angle) 
	    s2 = maps(numR, min_range, mid_range - 1, max_angle, mid_angle)
	else:
	    s1 = maps(numR, mid_range + 2, max_range, mid_angle, max_angle)
	    s2 = maps(numR, mid_range + 2, max_range, mid_angle, min_angle)
	#print out coordinates/angles of servo motors
	print("\n")
	print("output to servo1: ")
	print(s1)
	print("\n")
	print("output to servo2: ")
	print(s2)
	print("\n")
	#motors turn according to signal and go back to original position
	#servo1.write(s1)
	#servo2.write(s2)
	SetAngle(s1, s2)
	
    elif (numR <= 5 and numR >= 4 and (numU > 5 or numU < 4)): #if U is greater than 5 or less than 4 and R value is 4-5, only U signal is sent
	if (numU >= 0 and numU < 4):
	  s1 = maps(numU, min_range, mid_range - 1, min_angle, mid_angle); #move down
	  s2 = maps(numU, min_range, mid_range - 1, min_angle, mid_angle);
	else:
	  s1 = maps(numU, mid_range + 2, max_range, mid_angle, max_angle); #move up
	  s2 = maps(numU, mid_range + 2, max_range, mid_angle, max_angle);
	
	#print out coordinates/angles of servo motors
	print("\n")
	print("output to servo1: ")
	print(s1)
	print("\n")
	print("output to servo2: ")
	print(s2)
	print("\n")
	#motors turn according to signal and go back to original position
	#servo1.write(s1)
	#servo2.write(s2)
	SetAngle(s1, s2)
    
    elif ((numU <= 5 and numU >= 4) and (numR <= 5 and numR >= 4)): #If both R and U values are 4-5, only one signal is sent to bring fins back to original position
	s1 = 90
	s2 = 90
	#print out coordinates/angles of servo motors
	print("\n")
	print("output to servo1: ")
	print(s1)
	print("\n")
	print("output to servo2: ")
	print(s2)
	print("\n")
	#motors turn according to signal and go back to original position
	#servo1.write(s1)
	#servo2.write(s2)
	SetAngle(s1, s2)

def main(args):
    return 0

if __name__ == '__main__':
    ser = serial.Serial('/dev/ttyACM0', 9600)
    ser.flush
    sense = SenseHat()
    servoPin1=26
    servoPin2=16
    GPIO.setup(servoPin1, GPIO.OUT)
    GPIO.setup(servoPin2, GPIO.OUT)
    pwm1=GPIO.PWM(servoPin1, 50)
    pwm2=GPIO.PWM(servoPin2, 50)
    pwm1.start(0)
    pwm2.start(0)

	

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
