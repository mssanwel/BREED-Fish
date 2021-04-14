/*
   Fish encoder + servo fins (receiver side)

   Description: Receive 4 signals from transmittor
         R (Left-right fins), U (Up-down fins), T (Turn tail), P (power))

   Edited by Tasnia, Brianna, Heidi, Allison
*/

#include <SD.h>
#include <Servo.h>
#include <SPI.h>
#include <Wire.h>
#define SERIAL_RX_BUFFER_SIZE 8
//Servo
Servo servo1;
Servo servo2;
int servoPin1 = 5;
int servoPin2 = 3;
String letter;
int numR = 100, numU = 100, numT = 100, numP = 100;
int s1, s2;
int initial1 = 90;
int initial2 = 90;
int min_angle = 45;
int max_angle = 135;
int mid_angle = 90;
int min_range = 0;
int max_range = 9;
int mid_range = 4;
bool check;

//Encoder
unsigned int pwm_Pin = 9;
unsigned int motor_Pwm = 0;
char cmd[11];    //to store the signal from transmitter
bool red_Flag = LOW;
bool on = LOW;
unsigned long int timer1 = 0;
unsigned long int timer2 = 0;
long int counter = 0;
long int runtime = 0;
long int timer = 0;
float timeStep = 0;
long int encoderCounter = 0;
double angularVelocity[3] = {0, 0, 0};
int turnVal = 5;        //Stores commmand value for control left and right. values 1-4 are left, 5 is straight, 6-9 are right
int oldTurnVal = 5;
int encoderVal = 0;         //stores value of encoder at any given time. Updates from interrupt.
int power = 9;          //Stores power value from controller
int oldPower = 5;
long int tailTimer1 = millis(); //timer for kill switch
long int tailDelay1 = 2000;
int encoderPin0   =  29;
double diff = 1.0;              //to store the differential value corresponding to the signal
double stepDiff = 0.2;          //sets the differential value
int highcutoff, lowcutoff, offset;
int motor_temp = 5;

void setup() {
  Serial.begin(9600, SERIAL_8O1);
  Serial1.begin(9600);
  pinMode(servoPin1, OUTPUT);
  pinMode(servoPin2, OUTPUT);
  pinMode(pwm_Pin, OUTPUT);
  pinMode(13, OUTPUT);
  servo1.write(initial1);
  servo2.write(initial2);
  servo1.attach(servoPin1);
  servo2.attach(servoPin2);
  for (int encoderPin = encoderPin0; encoderPin <= 47; encoderPin = encoderPin + 2) { //absolute encoder pin setup
    pinMode(encoderPin, INPUT_PULLUP);
    Serial.println(encoderPin);
  }
  while (!Serial.read());
  timer = micros();
  Serial.println("Encoder Pins Initialized!");
  Serial.println("Setup is complete! Click to begin the program....");
  encoder();      //reads the offset value
  delay(1000);
  offset = encoderVal;
  Serial.print("Offset= ");
  Serial.println(offset);

  Serial.println("I'm ready for the code!");
}

//function for check sum
bool checkSum(char incomingByte, int siglen, char cmd[]) {
  Serial.println();
  int number = (String(cmd[8]) + String(cmd[9]) + String(cmd[10])).toInt();
  if (cmd[0] + cmd[1] + cmd[2] + cmd[3] + cmd[4] + cmd[5] + cmd[6] + cmd[7] == number) {
    Serial.print("Checksum satisfied, the signal is: ");
    //    for (int i = 0; i < 8; i++) {
    //      Serial.print(cmd[i]);
    //    }
    Serial.print(cmd[0]);
    Serial.print(cmd[1]);
    Serial.print(cmd[2]);
    Serial.print(cmd[3]);
    Serial.print(cmd[4]);
    Serial.print(cmd[5]);
    Serial.print(cmd[6]);
    Serial.print(cmd[7]);
    Serial.println("");
    check = true;
  } else {
    check = false;
  }
  return check;
}

void loop() {
  char incomingByte;
  int siglen = 0;  //to store the length of the incoming signal
  char cmd[11];
  if (Serial1.available() > 0) {  //if more than 2 bytes available to read in the buffer
    Serial.print("I'm receiving: ");
    // read the incoming byte:
    incomingByte = Serial1.read();
    while (incomingByte != '?') {  //read char by char until we know the end of signal is reached indicated by the identifier '?'
      cmd[siglen] = incomingByte;
      Serial.print(incomingByte);
      siglen++;
      incomingByte = Serial1.read();
    }

    check = checkSum(incomingByte, siglen, cmd);
  }
  if (check == true) {
    tailTimer1 = millis();
    //array[0] is 'R' or 'U' or 'T' or 'P' - convert to string, array[1:] are the numbers - convert to integer
    String w = String(cmd);
    numR = String(w.substring(1, 2)).toInt();
    numU = String(w.substring(3, 4)).toInt();
    numT = String(w.substring(5, 6)).toInt();
    numP = String(w.substring(7, 8)).toInt();
    Serial.println();
    Serial.print("R "); Serial.println(numR);
    Serial.print("U "); Serial.println(numU);
    Serial.print("T "); Serial.println(numT);
    Serial.print("P "); Serial.println(numP);

    power = numP ; // power signal
    turnVal = numT; // tail turning signal

    if ((numR > 5 || numR < 4) && (numU > 5 || numU < 4)) { //if both R and U signals are not 4-5, both signals written to servo 
      if (numU >= 0 && numU < 4) {
        s1 = map(numU, min_range, mid_range - 1, min_angle, mid_angle); //move down
        s2 = map(numU, min_range, mid_range - 1, min_angle, mid_angle);
      } else {
        s1 = map(numU, mid_range + 2, max_range, mid_angle, max_angle); //move up
        s2 = map(numU, mid_range + 2, max_range, mid_angle, max_angle);
      }

      //print out coordinates/angles of servo motors
      Serial.print("\n");
      Serial.print("output to servo1: ");
      Serial.print(s1);
      Serial.print("\n");
      Serial.print("output to servo2: ");
      Serial.print(s2);
      Serial.print("\n");
      //motors turn according to signal and go back to original position
      servo1.write(s1);
      servo2.write(s2);
      delay(300);
      if (numR >= 0 && numR < 4) {
          s1 = map(numR, min_range, mid_range - 1, min_angle, mid_angle); //left
          s2 = map(numR, min_range, mid_range - 1, max_angle, mid_angle);
      } else {
          s1 = map(numR, mid_range + 2, max_range, mid_angle, max_angle);
          s2 = map(numR, mid_range + 2, max_range, mid_angle, min_angle);
      }
        //print out coordinates/angles of servo motors
        Serial.print("\n");
        Serial.print("output to servo1: ");
        Serial.print(s1);
        Serial.print("\n");
        Serial.print("output to servo2: ");
        Serial.print(s2);
        Serial.print("\n");
        //motors turn according to signal and go back to original position
        servo1.write(s1);
        servo2.write(s2);
      }
    

    else if ((numR > 5 || numR < 4) && numU <= 5 && numU >= 4) { //if R is greater than 5 or less than 4 and U value is 4-5, only R signal is sent
      if (numR >= 0 && numR < 4) {
        s1 = map(numR, min_range, mid_range - 1, min_angle, mid_angle); //left
        s2 = map(numR, min_range, mid_range - 1, max_angle, mid_angle);
      } else {
        s1 = map(numR, mid_range + 2, max_range, mid_angle, max_angle);
        s2 = map(numR, mid_range + 2, max_range, mid_angle, min_angle);
      }
      //print out coordinates/angles of servo motors
      Serial.print("\n");
      Serial.print("output to servo1: ");
      Serial.print(s1);
      Serial.print("\n");
      Serial.print("output to servo2: ");
      Serial.print(s2);
      Serial.print("\n");
      //motors turn according to signal and go back to original position
      servo1.write(s1);
      servo2.write(s2);
    }
    else if (numR <= 5 && numR >= 4 && (numU > 5 || numU < 4)) { //if U is greater than 5 or less than 4 and R value is 4-5, only U signal is sent
      if (numU >= 0 && numU < 4) {
        s1 = map(numU, min_range, mid_range - 1, min_angle, mid_angle); //move down
        s2 = map(numU, min_range, mid_range - 1, min_angle, mid_angle);
      } else {
        s1 = map(numU, mid_range + 2, max_range, mid_angle, max_angle); //move up
        s2 = map(numU, mid_range + 2, max_range, mid_angle, max_angle);
      }

      //print out coordinates/angles of servo motors
      Serial.print("\n");
      Serial.print("output to servo1: ");
      Serial.print(s1);
      Serial.print("\n");
      Serial.print("output to servo2: ");
      Serial.print(s2);
      Serial.print("\n");
      //motors turn according to signal and go back to original position
      servo1.write(s1);
      servo2.write(s2);
    }
    else if ((numU <= 5 && numU >= 4) && (numR <= 5 && numR >= 4)) { //If both R and U values are 4-5, only one signal is sent to bring fins back to original position
      s1 = 90;
      s2 = 90;
      //print out coordinates/angles of servo motors
      Serial.print("\n");
      Serial.print("output to servo1: ");
      Serial.print(s1);
      Serial.print("\n");
      Serial.print("output to servo2: ");
      Serial.print(s2);
      Serial.print("\n");
      //motors turn according to signal and go back to original position
      servo1.write(s1);
      servo2.write(s2);
    }
    


    killswitch();
    encoder();    //reads the current position of the shaft

    if ((offset + 512) > 1023) {
      lowcutoff = offset + 512 - 1023;
      highcutoff = offset;
    }
    else {
      highcutoff = offset + 512;
      lowcutoff = offset;
    }
    Serial.print("highcutoff:");
    Serial.print(highcutoff);
    Serial.print("lowcutoff:");
    Serial.println(lowcutoff);

    // tail turning left
    if ((turnVal >= 1) && (turnVal <= 4)) {
      Serial.println("Turning Left");
      diff = 1 - (stepDiff * (5 - turnVal));
      Serial.println(diff);
      if ((encoderVal < highcutoff) && (encoderVal > lowcutoff)) {
        motor_Pwm = map(power, 0, 9, 0, 255);
      }
      else {
        motor_Pwm = map(power, 0, 9, 0, 255) * diff;
      }
    }

    // tail going start
    else if (turnVal == 5) {
      Serial.println("Going straight");
      motor_Pwm = map(power, 0, 9, 0, 255); //maps the value received (0-9) to (0-255)
    }

    // tail turning right
    else if ((turnVal >= 6) && (turnVal <= 9)) {
      Serial.println("Turning Right");
      diff = 1.0 - (stepDiff * (turnVal - 5));
      if ((encoderVal < highcutoff) && (encoderVal > lowcutoff)) {
        motor_Pwm = map(power, 0, 9, 0, 255) * diff;
      }
      else {
        motor_Pwm = map(power, 0, 9, 0, 255);
      }
    }
    //print out encoder output and power


    analogWrite (pwm_Pin, 255);
    Serial.print("motor pwm: ");
    Serial.println(motor_Pwm);
    oldTurnVal = turnVal;
    oldPower = power;
    Serial.print("Motor:");
    Serial.print(motor_Pwm);

    motor_temp = motor_Pwm;
  } else {
    Serial.print("Rejected!");
  }
  delay(500);
}



//encoder function
void encoder() {
  // returns the position of the shaft (0-1023)
  // put your main code here, to run repeatedly:
  int a[10];
  int b[10];
  for (int n = 0 ; n < 10 ; n++ ) {
    a[n] = !digitalRead(encoderPin0 + 2 * n);
  }
  b[9] = a[9];
  for (int i = 1; i < 10; i++) {
    b[9 - i] = b[9 - i + 1] ^ a[9 - i]; // xor
  }
  float encoderValueRaw = 512.0 * b[9] + 256.0 * b[8] + 128.0 * b[7] + 64.0 * b[6] + 32.0 * b[5] + 16.0 * b[4] + 8.0 * b[3] + 4.0 * b[2] + 2.0 * b[1] + b[0];
  Serial.print("encodervalue : ");
  Serial.println(encoderValueRaw);
  encoderVal = encoderValueRaw;
}

//kill switch if new signal is not received for 3 seconds
void killswitch() {
  if (millis() - tailTimer1 > 3000) {
    motor_Pwm = 0;
  }
}
