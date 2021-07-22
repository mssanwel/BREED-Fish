/* Fish control in the main tube
    Complete on 23/05/2018 Version 3.6


  Changelogs:

  Fixed encoder angle, because the angle is relative to the starting point, we need to compensate for the 0 error.
  Fixed turning code to be relative to the duty cycle as opposed to a fix turning ratio.
  This is the code with the HEAVE functionality as well, also known as the heave function.

  Remember to set the encoder angle everytime we change the mechanism. The code assumes that the encoder angle is
  0 at the neutral position
  the motor rotates anti-clockwise.

  Currently Using Serial3.
  Serial would be used for debugging purposes.

  Turning Code Disabled to Test the HEAVE CODE.

  Next step, ask arduino to read paramters from a text file!

*/
#include <SD.h>
#include <Servo.h>
#include <SPI.h>
#include <Wire.h>
// Function declaration
float currentSensor();


// Current Sensor Variables

float stepI = 0.12 ; // The ACS 723 has it at ~0.012, 10 times better.
float zeroI = 512;
float current = 0;
const int     cur_sensorPin  = A0;
unsigned const int encoderInitial = 428;
char          inComingbyte[12];
char          sendData[12];
char          speedC[4];
char          dutyC[4];
char          minP[4];
char          heaveA[4];
char          receivedTurn;                        //received turning signal
bool          tubeState      = 1;
const int     pwmPin         = 9;
const int     inc_encoderPin = 2;
unsigned int  pwmLimit       = 0;                  //received dutycycle limit
unsigned int  receivedDutyCycle   = 0;             //received velocity(dutycycle if pid is not used)
unsigned int  right_count    = 0;                  //how many times turning right signal are received
unsigned int  left_count     = 0;                  //how many times turning left signal are received
unsigned int  ref_angle      = 0;                  //motor shaft position when turning signal is received
unsigned long ref_rotation   = 0;                  //motor shaft rotation times when turning signal is received
unsigned long rotation       = 0;                  //
unsigned long lastReciveTime = 0;                  //
volatile unsigned int  encoderValueRaw = 0;        //decimal reading from absolute encoder
unsigned int  pwmOutput = 0;                       //
volatile float angle = 0.0;                        //current motor shaft position
float lastAngle = 0.0;                             //last motor shaft position
float velocity = 0.0;                              //motor rotational velocity(rps)
float turnDifferential = 0.05;                     //5% lower on one side, and 5% higher on the other side.
unsigned int deltaT = 10;                          //time constant
unsigned long t = 0;
unsigned long tMax = 0;
//const int enablePin = 4;                        //AMP enable pin
//volatile unsigned long encoder0Pos = 0;         //Count plus

// Gait Control Variables
const float maxStallPwm = 0.15;      // This is as a percentage of the assigned duty cycle
int minPwm = 0;                      // This is the bare minimum pwm signal to get the motor moving under the weight of water and tail PWM range [0,255]
float contPwmOutput = 0;
int heaveAngle = 0;                  // This is the compensate amount where the minimum PWM will be in effect. Having it as 30 means that the low PWM range is 60 degrees. Should be less than 45 degrees.
int minPwmLimit = 38;
int heaveAngleLimit = 40;
unsigned int receivedminPwm;
unsigned int receivedheaveAngle;





void setup() {
 // TCCR2B = TCCR2B & 0b11111000 | 0x01;
  //TCCR1B = TCCR1B & 0b11111000 | 0x03;
  Serial.begin(115200);
 // Serial.begin(19200, SERIAL_8O1); 
  //for (int encoderPin = 29; encoderPin <= 47; encoderPin = encoderPin + 2 ) { //absolute encoder pin setup
    //pinMode(encoderPin, INPUT_PULLUP) ;
  //}
  //pinMode(pwmPin, OUTPUT);                  //motor control pwm signal pin
  //zeroI = analogRead(cur_sensorPin);
  //delay(1);
  // Timer0 is already used for millis() - we'll just interrupt somewhere
  // in the middle and call the "Compare A" function below
  //OCR0A = 0xAF;
  //TIMSK0 |= _BV(OCIE0A);   TIMSK3 = (TIMSK3 & B11111110) | 0x01;

  
  //Update (Compare Timer interrupt) timer 2
   //TIMSK3 = (TIMSK3 & B11111110) | 0x01;
   //TCCR3B = (TCCR3B & B11111000) | 0x03; //Settings for clock divisor 0x01 : 1, 0x02 : 8, 0x03 : 32, 0x04 : 64, 0x05 : 128, 0x06 : 256, 0x07 : 1024
}




void loop() {
  //float deltat = 0;
  //t = micros();
  updateData();
  //current = currentSensor();
  //absoluteEncoder();
  //pwmOutput = map(receivedDutyCycle, 0, 99, 0, 255);  // very important to do. Probably want to get rid of it.
  //heaveAngle = receivedheaveAngle;
  //minPwm = receivedminPwm;
  //contPwmOutput = gaitControl(pwmOutput);   // contPwmOutput stands for the controlled PWM OUTPUT, that is after it has undergone, feedback, gait,or any turning requirements.
  
  //analogWrite(pwmPin, contPwmOutput );
  //deltat = (micros() - t);
  //Serial.println("loop() execution time is ");
  //Serial.println(deltat);
  //speedFromAbsoluteEncoder();
  /*if (deltat > tMax) tMax = deltat;
  Serial.print("\t");
  Serial.print(current);
  Serial.print("\t");
  Serial.print(angle / 20);
  Serial.print("\n");*///-> uncomment afterwards
  //delay(50);
}





/*ISR(TIMER3_OVF_vect){
  //Variable for measuring program execution time
  int progStart = 0;
  int progEnd = 0;
  progStart = micros();
  //Time output 4 MicroSeconds when no function being executed below.
  //Total Time=392-1484 Update = 468
  current = currentSensor();//Time=180-1200 Update=196-208
  //absoluteEncoder();//Time=272-276
  /*pwmOutput = map(receivedDutyCycle, 0, 99, 0, 255);  // very important to do. Probably want to get rid of it.
  heaveAngle = receivedheaveAngle;
  minPwm = receivedminPwm;
  contPwmOutput = gaitControl(pwmOutput);   // contPwmOutput stands for the controlled PWM OUTPUT, that is after it has undergone, feedback, gait,or any turning requirements. 
  progEnd = micros();
  Serial.println("ISR execution time is...................................................... ");
  Serial.println(progEnd-progStart);
}*/







void updateData() {//data update from controller
    if (Serial.available() >0) {
      int Data=Serial.read();
      //Serial.println(Data);
      Serial.println("recieved");
    
       if (inComingbyte[0] == 'c') {
        tubeState = 0;
        for (int i = 0; i < 2; i++) {
          dutyC[i]  = inComingbyte[i + 1];
          speedC[i] = inComingbyte[i + 3];
          minP[i] = inComingbyte[i + 5];
          heaveA[i]  = inComingbyte[i + 7];
        }
        pwmLimit     = atoi(dutyC);
        receivedDutyCycle = atoi(speedC);
        receivedminPwm = atoi(minP);
        receivedheaveAngle = atoi(heaveA);
        receivedTurn        = inComingbyte[9];
        if (receivedDutyCycle > pwmLimit) { 
          receivedDutyCycle = pwmLimit;
        }
        if (receivedminPwm > minPwmLimit) {
          receivedminPwm = minPwmLimit;
        }
        if (receivedheaveAngle > heaveAngleLimit) {
          receivedheaveAngle = heaveAngleLimit;
        }
        lastReciveTime = millis();
      }
      //while(Serial3.available()>6) Serial3.read();
    }

//kill motor if lose contact with controller for more than 3 seconds
}
    //handshake back with controller
    sprintf(sendData, "f%02u%02u%02u%02u%ce", 5, 5, 5, 5, 5);
    Serial.print(sendData);
    tubeState = 1;
    }
}






/*
void absoluteEncoder() {              //get angle from absolute encoder
  //Variable for measuring program execution time
  //int progStart = 0;
  //int progEnd = 0;
  //progStart = micros();
  volatile int a[10];
  volatile int b[10];
  for (int n = 0; n < 10; n++) {
    a[n] = !digitalRead(2 * n + 29);
  }
  b[9] = a[9];                              //Q?????
  for (int i = 1; i < 10; i++) {
    b[9 - i] = b[9 - i + 1] ^ a[9 - i];
  }
  encoderValueRaw = 512.0 * b[9] + 256.0 * b[8] + 128.0 * b[7] + 64.0 * b[6] + 32.0 * b[5] + 16.0 * b[4] + 8.0 * b[3] + 4.0 * b[2] + 2.0 * b[1] + b[0];
  //Serial.println(encoderValueRaw);

  // here's the biggest fix, it's the return angle of the encoder wasn't right before.
  if (encoderValueRaw >= encoderInitial) angle  = (encoderValueRaw - encoderInitial) * 360.0 / 1024.0;
  else angle = (encoderValueRaw + 1023 - encoderInitial) * 360.0 / 1024.0;
  //progEnd = micros();
  //Serial.println("absoluteEncoder() execution time is ");
  //Serial.println(progEnd-progStart);
}







void speedFromAbsoluteEncoder() {      //calculate velocity from absolute encoder
  //Variable for measuring program execution time
  //int progStart = 0;
  //int progEnd = 0;
  //progStart = micros();
  float angle_dif = angle - lastAngle;
  if (angle_dif > 300) {        //motor rotate clockwise
    angle_dif = angle_dif - 360;
    rotation++;
  }
  else if (angle_dif < -300) {  //motor rotate countclockwise
    angle_dif = angle_dif + 360;
    rotation++;
  }
  velocity = abs(angle_dif) * 1000.0 / deltaT / 360.0;
  //Serial.println(velocity);
  lastAngle = angle;
  //progEnd = micros();
  //Serial.println("speedFromAbsoluteEncoder() execution time is ");
  //Serial.println(progEnd-progStart);
}






float fishTurning(float y) {
  // Changes the PWM Output for turning.
  /* The turning function simply tells the fish to turn when it receives the signal, and it stops turning
      if it does not receive any signal. States from the controller are r,'0',l.
     From the flow, the aerodynamics dictate that the fish turns right.
    Another way to see it is the orientation of the head. For power strokes,
    if you paddle on the left of a boat, you will turn right.

    Assumes 0 is eq. point, tail starts going right.
    (i) Right Drag stroke (0-90) = RIGHT DRAG
    (ii) Right Power Stroke (90-180) = LEFT POWER
    (iii)Left Drag stroke (180-270) = LEFT DRAG
    (iv) Left Power Stroke (270-360) = RIGHT POWER
  */
  /*//Variable for measuring program execution time
  //int progStart = 0;
  //int progEnd = 0;
  //progStart = micros();
  float pwmHigh = y * (1 + turnDifferential);
  float pwmLow = y * (1 - turnDifferential);
  if (pwmLow < 0) pwmLow = 0;
  float pwmOutput = 0;
  
  if (receivedTurn == 'r') {
    //Power up third and fourth quadrants.

    //Drag Stroke
    if (angle > 90 && encoderValueRaw <= 180) {
      pwmOutput = pwmLow;
      return pwmOutput;
      //pwmOutput = constrain(pwmOutput,0, (receivedDutyCycle + turnDifferential));
    }
    else if (angle > 0 && encoderValueRaw <= 90) {
      pwmOutput = pwmHigh;
      return pwmOutput;
      //pwmOutput = constrain(pwmOutput,(receivedDutyCycle - turnDifferential),99);
    }
  }
  else if (receivedTurn == 'l') {
    if (angle > 270 && angle <= 360) {
      pwmOutput = pwmLow;
      return pwmOutput;
      //pwmOutput = constrain(pwmOutput,0, (receivedDutyCycle + turnDifferential));
    }
    else if (angle > 180 && encoderValueRaw <= 270) {
      pwmOutput = pwmHigh;
      return pwmOutput;
      //pwmOutput = constrain(pwmOutput,(receivedDutyCycle - turnDifferential),99);
    }
  }
  else {
    pwmOutput = y;
    return pwmOutput;
  }
  //progEnd = micros();
  //Serial.println("fishTurning() execution time is ");
  //Serial.println(progEnd-progStart);
}






float avgCurrent = 0;
int counterA = 0;
float curA[5] = {0};
float currentSensor() {
  
  int x = analogRead(A0);
  float current = (x - zeroI) * stepI;

  //moving average filter for the current
  curA[counterA] = current;
  counterA++;
  if (counterA > 5) counterA = 0; // reset the counter and replace the old data with new ones

  for (int i = 0; i < 5; i++)
  {
    //sum of the array current
    avgCurrent += curA[i];
  }
  avgCurrent = avgCurrent / 5;

  return avgCurrent; //  filter mode
}

// Interrupt Service Routine( for the Sensors)








float gaitControl(byte x) {

  // This code provides a sinosuidal output so that we can control the gait of the fish
  // Rather than having a huge jump, we hope to let the motor ease into it's stall point
  // Appends the pwmOutput of the
  
  //Variable for measuring program execution time
  //int progStart = 0;
  //int progEnd = 0;
  //progStart = micros();
  
  float pwmOutput = x * ( (1 - maxStallPwm) * abs(cos(angle * PI / 180)) + maxStallPwm); //get faster trig library

  //pwmOutput = fishTurning(pwmOutput);
  if (pwmOutput == 0)
  {
    pwmOutput = 0;
    return pwmOutput;
  }
  else if ((angle > 90 - heaveAngle && angle < 90 + heaveAngle) || (angle > 270 - heaveAngle && angle < 270 + heaveAngle)) pwmOutput = minPwm;
  else if (pwmOutput < minPwm) pwmOutput = minPwm;
  return pwmOutput;

  //progEnd = micros();
  //Serial.println("gaitControl() execution time is ");
  //Serial.println(progEnd-progStart);
}*/
