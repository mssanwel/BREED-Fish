/* 
    *  Fish Encoder
    *  
    *  Receives instructions from transmitter for turning and power values for the fish tail
    *  
    *  Modified 1 Mar 2021
    *  Modified by Allison, Brianna, Heidi
    *  
*/

//Arduino Pin Declarations
unsigned int pwm_Pin = 9;
unsigned int motor_Pwm = 0;

bool red_Flag = LOW;
bool on = LOW;
unsigned long int timer1 = 0;
unsigned long int timer2 = 0;

char cmd[11];    //to store the signal

long int counter =0;
long int runtime = 0;
long int timer = 0;
float timeStep = 0;
long int encoderCounter = 0;
double angularVelocity[3] = {0,0,0};
int turnVal=5;          //Stores commmand value for control left and right. values 1-4 are left, 5 is straight, 6-9 are right
int oldTurnVal=5;
int encoderVal=0;           //stores value of encoder at any given time. Updates from interrupt.
//int oldEncoderVal;
int power=5;            //Stores power value from controller
int oldPower=5;
long int tailTimer1=millis();
//long int tailTimer2=millis();   //time when power is reduced (when applicable)
long int tailDelay1=2000;
//long int tailDelay2=1000;       //time to occurrence of fish stalling
int encoderPin0   =  29;
double diff = 1.0;              //to store the differential value corresponding to the signal
double stepDiff = 0.1;          //sets the differential value

int highcutoff, lowcutoff, offset, num;

//bool slow=false;

void setup() {
 // Define the Serial ports, with Serial1 as the Receiver Port for Data from the transmitter. 
 // The RC is defined as 8 bits, Odd parity, 1 check digit. Default Arduino setting is SERIAL_8N1, where N means no parity.
 
 // Set the PWM frequency of Timer 2 to be 3kHz
 TCCR2B = TCCR2B & 0b11111000 | 0x01;

 //Define the output pins
 pinMode(pwm_Pin, OUTPUT);
 pinMode(13, OUTPUT);

  Serial1.begin(9600,SERIAL_8O1);
  Serial.begin(9600);
  //Serial.begin(115200);
  //Serial.println("Serial Port Initialized!");

 
  for(int encoderPin = encoderPin0; encoderPin <= 47;encoderPin = encoderPin + 2){   //absolute encoder pin setup
    pinMode(encoderPin,INPUT_PULLUP);
    Serial.println(encoderPin);
  }
  
  while(!Serial.read());
  timer = micros();
  

  Serial.println("Encoder Pins Initialized!");
  Serial.println("Setup is complete! Click to begin the program....");

  encoder();      //reads the offset value
  delay(1000);
  offset=encoderVal;
  Serial.print("Offset= ");
  Serial.println(offset);
}

void loop() {
    
  char incomingByte;
  int siglen = 0;  //to store the length of the incoming signal
  
  if (Serial1.available() > 0) {  //if more than 2 bytes available to read in the buffer
    Serial.print("I'm receiving: ");
    incomingByte = Serial1.read();
    while (incomingByte != '?') {  //read char by char until we know the signal is completely read indicated by the identifier '?'
      cmd[siglen] = incomingByte;
      Serial.print(incomingByte);
      siglen++;
      incomingByte = Serial1.read();
      Serial1.flush();
    }
  }

  Serial.println();
  if (checkSum(cmd)==true){
    //continues only if signal is valid
    
    tailTimer1 = millis();
    interpretSignal();
    //Kill switch
    killswitch();
    encoder();    //reads the current position of the shaft
    if (oldTurnVal != turnVal || oldPower != power){
    // sets the range of values for different motor power to generate tail motion corresponding to input signal
      if ((offset + 512) >1023){
       lowcutoff = offset+512-1023;
       highcutoff = offset;
      }
      else{
       highcutoff = offset + 512;
       lowcutoff = offset;
      }
      Serial.print("highcutoff:");
      Serial.print(highcutoff);
      Serial.print("lowcutoff:");
      Serial.println(lowcutoff);
      
      // Turning control Left
      if ((turnVal>=1) && (turnVal<=4)){
        Serial.println("Turning Left");
        diff = 1-(stepDiff*(5-turnVal));
        Serial.println(diff);
        if ((encoderVal < highcutoff) && (encoderVal > lowcutoff)){
          motor_Pwm = map(power, 0,9,0,255);
        }
        else {
          motor_Pwm = map(power, 0,9,0,255) * diff;
        }
      }
      
      // Going Straight
      else if (turnVal==5){
        Serial.println("Going straight");
        motor_Pwm = map(power, 0,9,0,255); //maps the value received (0-9) to (0-255)
      }
      
      // Turning control Right
      else if ((turnVal>=6) && (turnVal<=9)){
        Serial.println("Turning Right");
        diff = 1.0-(stepDiff*(turnVal-5));
        if ((encoderVal < highcutoff) && (encoderVal > lowcutoff)){
          motor_Pwm = map(power, 0,9,0,255) * diff;
        }
        else {
          motor_Pwm = map(power, 0,9,0,255);
        }
      }
      analogWrite (pwm_Pin, motor_Pwm);
      Serial.print("motor pwm: ");
      Serial.println(motor_Pwm);
      oldTurnVal = turnVal;
      oldPower = power;
//      oldEncoderVal = encoderVal;
    }
  }
}


void encoder() {
  // returns the position of the shaft (0-1023)
  // put your main code here, to run repeatedly:
  int a[10];
  int b[10];
  for(int n = 0 ; n<10 ; n++ ){
    a[n] = !digitalRead(encoderPin0 + 2*n);
  }
  b[9] = a[9];
  for(int i = 1;i<10;i++){
    b[9-i] = b[9-i+1]^a[9-i];  // xor
  }
  float encoderValueRaw = 512.0*b[9]+256.0*b[8]+128.0*b[7]+64.0*b[6]+32.0*b[5]+16.0*b[4]+8.0*b[3]+4.0*b[2]+2.0*b[1]+b[0];
  Serial.print("encodervalue : ");
  Serial.println(encoderValueRaw);
  encoderVal= encoderValueRaw;
}

//checkSum function
bool checkSum(char cmd[]){
  //checks whether the input is a valid signal
  String numcheck = String(cmd[2])+String(cmd[3])+String(cmd[4]);
  int number = numcheck.toInt();
  Serial.print("The check is: ");
  Serial.println(number);
  if (cmd[0]+cmd[1]==number){
    Serial.print("Inside the array there is: ");
    for(int i = 0; i < 2; i++) {
      Serial.print(cmd[i]);
    }
    return true;    //if signal is VALID
  }
  else {
    return false;   //if signal is CORRUPTED
  }
}

//interpret signal
void interpretSignal(){
   //array[0] is 'T' or 'U' - convert to string, array[1:] are the numbers - convert to integer  
   String w = String(cmd);
   String letter = String(w[0]);
   num = String(w.substring(1)).toInt();
   Serial.print("The number for movement is: ");
   Serial.println(num);
   if (letter=="T"){
      turnVal=num; // turning signal
   }
   if (letter=="P"){
      power = num ; // power signal    
   }
}

//kill switch if new signal is not received for 3 
void killswitch(){
  if (millis() - tailTimer1 > 3000) {
      motor_Pwm = 0; 
  }
}

//void fishStalling(){
//  if (slow==false){
//    tailTimer2 = millis();
//  }
//  if ((millis()-tailTimer2) >= tailDelay2 && (oldEncoderVal==encoderVal)){
//    motor_Pwm = map(power, 0,9,0,255);
//  }
//  else{
//    motor_Pwm = map(power, 0,9,0,255) * diff;
//  }
//  slow = true;
//  tailTimer2 = millis();
//}
