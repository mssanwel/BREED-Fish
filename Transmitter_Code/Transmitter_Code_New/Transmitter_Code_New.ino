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

long timer1 = 0;
long timerdiff = 0;

bool flag1 = true;
bool flag2 = false;

int counter1 = 0;

//variables to control the power and turn, which will be sent together in command array
//power contains information about the power from 0-9 depending on potpin value
//turn contains information about turning where 5 represents straight value
//6-9 represents intensity of turning left 
//4-1 represents information about right turn intensity
//9 is extreme left turn and 1 is extreme right turn
int power= 0;
int turn = 5;

//the command signal is the signal that sends information about what to do from controller
char command[3];
//using pointers so when we update power and turn, command gets updated automatically
//wait to implement until char and int variable can be sorted
//*command[0] = &power;
//*command[1] = &turn;

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
  Serial.begin(9600, SERIAL_8O1);
}

void loop() {
  //Read the Pot Pin every 200s and send a signal
  power = analogRead(potPinpower);
  delay(1);
  turn = analogRead(potPinturn);
  delay(1);
  if(millis() - timer1> 200 && flag1) {
    /////////////////////////////////////////////
    //for power/speed
    timer1 = millis();
    //delay to allow the ADC to finish the reading. 
    //Maps the potentiometer reading from 0-255 to 0-9 to shorten the transmission signal. Would be better if we could simply send a character. (0-255)
    // Sets the maximum and minimum value of the Pot value. 
    if (power > 900) 
      power = 1023;
    if (power < 150)
      power = 0;
    power = map(power , 0 , 1023 , 0, 9);
    if(turn >= 213 && turn <= 625)
       finalturn = 5;
    if (turn > 650) 
      finalturn = 7; 
    if (turn < 150)
      finalturn = 2;
    //Serial.print(turn);
    //Serial.print(" Final turn = ");
    //Serial.println(finalturn);
    //turn = map(turn , 0 , 1023 , 1, 9);
    ///////////////////////////////////////////////////
    //for turning
    /*
    //check if button pressed and last time it was pressed was more than half a second ago
    if(digitalRead(leftbutton) && (millis() - buttontimer > 500)) {
      //increase the left turning value but only if it is less than 9 and update the buttontimer
      if(turn < 9) {
        turn++;
      }
      buttontimer = millis();
    }
    else if (digitalRead(rightbutton) && (millis() - buttontimer > 500)) {
      //decrease the turning value to denote a turn to right but only if it is more than 0 and update the buttontimer
      if(turn > 0) {
        turn--;
      }
      buttontimer = millis();
    }
    
    if(Serial.availableForWrite() > 30) {
      if (turn > 9 || turn < 0) {
        turn = 5;
      }
    */
    if(Serial.availableForWrite() > 30) {  
      /*Serial.print("Writing a value now: power = ");
      Serial.print(power);
      Serial.print(" & turn = ");
      Serial.print(turn);
      Serial.print("   ");
      */
      command[0] = power + '0';
      command[1] = finalturn + 'A';    //convert turn to ASCII to avoid conflict from 65- 74
      Serial.print(command); //Ensures that it doesn't overwhelm the system and to give time for the RC to transmit. 
    }
  }
  // Stops the transmission after 60 iterations 60*250ms 
  if (power == 0 && counter1 < 60) 
    counter1++;
        
  if (++counter1 > 50){ 
    flag1 = LOW; // if the signal stays at zero, stop the transmission so as to not overwhelm the buffer on the other side with zeros.
    
  }
  //if your machine has stopped transmitting but now x is increased, then restart the transmission
  if (power > 10 && flag1 == LOW) {
    flag1 = HIGH;
    counter1 = 0;
  }
  
}
