#include <SD.h>
#include <Servo.h>
#include <SPI.h>
#include <Wire.h>
Servo servo1;
Servo servo2;
int servoPin1 = 3;
int servoPin2 = 2;
String letter;
int num=100;
int s1, s2;
int initial1=90;
int initial2=90;

int min_angle = 0;
int max_angle = 180;
int mid_angle = 90;
int min_range = 0;
int max_range = 9;
int mid_range = 4;


void setup() {
  Serial1.begin(9600);
  Serial.begin(9600);
  pinMode(servoPin1, OUTPUT);
  pinMode(servoPin2, OUTPUT);
  servo1.write(initial1);
  servo2.write(initial2);
  servo1.attach(servoPin1);
  servo2.attach(servoPin2);
}
void loop() {
  char incomingByte;
  int siglen = 0;  //to store the length of the incoming signal
  char cmd[11];
  if (Serial1.available() > 0) {  //if more than 2 bytes available to read in the buffer
    Serial.print("I'm receiving: ");
    // read the incoming byte:
    incomingByte = Serial1.read();
    while (incomingByte != '?') {  //read char by char until we know the signal is completely read indicated by the identifier '?'
      cmd[siglen] = incomingByte;
      Serial.print(incomingByte);
      siglen++;
      incomingByte = Serial1.read();
    }
    Serial.println();
    String numcheck = String(cmd[2])+String(cmd[3])+String(cmd[4]);
    int number = numcheck.toInt();
  if (cmd[0]+cmd[1]==number){
      Serial.print("Inside the array there is: ");
      for(int i = 0; i < 2; i++) {
        Serial.print(cmd[i]);
      }
    //array[0] is 'T' or 'U' - convert to string, array[1:] are the numbers - convert to integer
    String w = String(cmd);
    String letter = String(w[0]);
    num = String(w.substring(1,2)).toInt();
    Serial.println();
    Serial.print("The number for movement is: ");
    Serial.println(num);
    if (letter=="T") { //for turning the fins left and right (servo motors turn in opposite directions)
      if (num>=min_range && num<mid_range) {
        s1=map(num,min_range, mid_range -1, min_angle, mid_angle); //left
        s2=map(num,min_range, mid_range -1,max_angle, mid_angle);
      }
      else if (num>mid_range +1 && num<=max_range){ //right
        s1=map(num,mid_range + 2, max_range, mid_angle, max_angle);
        s2=map(num, mid_range + 2, max_range, mid_angle, min_angle);
      }
      
//      else if (num>=4 and num<=5){ //no movement
//        s1=90;
//        s2=90;
//      }
    }
    
    if (letter=="U") { //for moving the fins up and down (servo motors turn in same directions)
      if (num>=0 && num<4) {
        s1=map(num, min_range, mid_range -2, min_angle, mid_angle); //left
        s2=map(num, min_range, mid_range -1, min_angle, mid_angle);
      }
      else if (num>5 && num<=9){ //right
        s1=map(num,mid_range + 2,max_range,mid_angle, max_angle);
        s2=map(num,mid_range+2,max_range,mid_angle,max_angle);
      }
      
//      else if (num>=4 and num<=5){ //no movement
//        s1=90;
//        s2=90;
//      }
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
   
    delay(20);
  }
  }
}
