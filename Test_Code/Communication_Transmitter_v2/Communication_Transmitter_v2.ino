  /*
 * Basic code written to test the basic functionality of Radio Control Modules (Serial based)
 * Allows us to check whether the hardware is working correctly
 * 
 * This is on the transmitter side, which transmits a signal
 * Strings or char arrays both can be sent using serial.print or serial.write
 * Written by Saad Shahid and Taha Abid 1st March 2021
 * 
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
      Serial1.(testval2);
      Serial.print(testval2); 
//    }
  }
}
