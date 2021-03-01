/*
 * Simple Transmitter Code 
 * 
 * Receives a value from the potentiometer connected at A0 on the Arduino Uno, and transmit the information to 
 * the Receiver. 
 * 
 * Currently setup to send a signal every 30ms (10Hz) 
 * Human Reaction time is 300ms, this is well below it. 
 */

char testval[] = {'a','b','c'};
String testval2 = "abcdef";
int timer1 = 0;
void setup() {
  // put your setup code here, to run once:
  Serial1.begin(9600, SERIAL_8O1);
  Serial.begin(9600);
  pinMode(INPUT, 7);
}

void loop() {  
  //Read the Pot Pin every 200s and send a signal
  if(millis() - timer1 > 200){
    timer1 = millis();
//    if(Serial.availableForWrite() > 30) {
      Serial1.print(testval2);
      Serial.print(testval2); 
//    }
  }
}
