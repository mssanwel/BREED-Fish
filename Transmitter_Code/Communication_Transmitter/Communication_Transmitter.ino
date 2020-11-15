/*
 * Simple Transmitter Code 
 * 
 * Receives a value from the potentiometer connected at A0 on the Arduino Uno, and transmit the information to 
 * the Receiver. 
 * 
 * Currently setup to send a signal every 30ms (10Hz) 
 * Human Reaction time is 300ms, this is well below it. 
 */

int potPin = A0;
int potSweep = 0;

long timer1 = 0;
long timer2 = 0;
long timerdiff = 0;

bool flag1 = true;
bool flag2 = false;

int counter1 = 0;

int x=0;

void setup() {
  pinMode(potPin, INPUT);
  // put your setup code here, to run once:
  Serial.begin(9600, SERIAL_8O1);
}

void loop() {
  

  //Read the Pot Pin every 200s and send a signal
  x = analogRead(potPin);
  if(millis() - timer1> 200 && flag1){
    timer1 = millis();
    //delay to allow the ADC to finish the reading. 
    //Maps the potentiometer reading from 0-255 to 0-9 to shorten the transmission signal. Would be better if we could simply send a character. (0-255)
    // Sets the maximum and minimum value of the Pot value. 
    if (x > 1000)x = 1023;
    if (x < 10)   x = 0;
    int y = map(x , 0 , 1023 , 0, 9);
    if(Serial.availableForWrite() > 30) {
      //Serial.print("Writing a value now");
      Serial.print(y); //Ensures that it doesn't overwhelm the system and to give time for the RC to transmit. 
    }
  }
  // Stops the transmission after 60 iterations 60*250ms 
  if (x == 0 && counter1 < 60) 
    counter1++;
        
  if (++counter1 > 50){ 
    flag1 = LOW; // if the signal stays at zero, stop the transmission so as to not overwhelm the buffer on the other side with zeros.
  }
  //if your machine has stopped transmitting but now x is increased, then restart the transmission
  if (x > 10 && flag1 == LOW) {
    flag1 = HIGH;
    counter1 = 0;
  }
  
}
