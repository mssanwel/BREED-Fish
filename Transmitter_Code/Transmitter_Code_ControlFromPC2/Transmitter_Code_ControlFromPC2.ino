/*
 * Simple Transmitter Code 
 * 
 * Receives a value from the potentiometer connected at A0 on the Arduino Uno, and transmit the information to 
 * the Receiver. 
 * 
 * Currently setup to send a signal every 30ms (10Hz) 
 * Human Reaction time is 300ms, this is well below it. 
 */ 

int potPinpower = A0;
int potPinturn = A4;
int potSweep = 0;

long timer1 = millis();
long timerdiff = 0;

bool flag1 = true;
bool flag2 = false;

int counter1 = 0;
String temp;
//variables to control the power and turn, which will be sent together in command array
//power contains information about the power from 0-9 depending on potpin value
//turn contains information about turning where 5 represents straight value
//6-9 represents intensity of turning left 
//4-1 represents information about right turn intensity
//9 is extreme left turn and 1 is extreme right turn
int power= 0;
int turn = 5;

//the command signal is the signal that sends information about what to do from controller
String command[3];
//using pointers so when we update power and turn, command gets updated automatically
//wait to implement until char and int variable can be sorted
//command[0] = 5 + '0';
//command[1] = 5 + 'A'; 

#define leftbutton b4
#define rightbutton b3

int finalturn;
int b3 = 10;
int b4 = 9;
//to keep track of button press
long buttontimer = 0;

void setup() {
  pinMode(potPinpower, INPUT);
  pinMode(potPinturn, INPUT);
  // put your setup code here, to run once:
  //pinMode( b3, INPUT); //right button
  //pinMode( b4, INPUT); //left button
  Serial1.begin(9600, SERIAL_8O1);
  Serial.begin(9600);
  temp="0";
}

void loop() {
  //Read the Pot Pin every 200s and send a signal
  //Serial.println("Running...");
  if (Serial.available() > 0) {
      temp=Serial.readString();
      Serial.println(temp);
      Serial.println("On"); 
      if (temp>="A" and temp<="I"){
        Serial.println("Turn command accepted");  
      }
      if (temp>="0" and temp<="9"){
        Serial.println("Power command accepted");
      }
  }
  
  if((millis() - timer1) > 500) {
    //Serial.println("Working..");
    
    if(Serial1.availableForWrite() > 0) {  
      Serial1.print(temp); //Ensures that it doesn't overwhelm the system and to give time for the RC to transmit.
      Serial.println(temp);
    }
    //Serial.println(temp);
    timer1=millis();
  }
  //Serial.println(temp);
  //delay(1000);
}
