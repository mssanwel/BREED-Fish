  /*
 * Basic code written to test the basic functionality of Radio Control Modules (Serial based)
 * Allows us to check whether the hardware is working correctly
 * 
 * This is on the receiver side, which receives a signal and displays on the Serial monitor
 * Receiver should be an Arduino Mega, as they have multiple Serial ports.
 * The signal can be read as a char, int or String but use readString or read to read String or a char respectively.
 * Written by Saad Shahid and Taha Abid 1st March 2021
 * 
 */
#include <SD.h>
#include <Servo.h>
#include <SPI.h>
#include <Wire.h>
void setup() {
  Serial.begin(9600);
  Serial1.begin(9600);
}
void loop() {
  //Serial.println(“Running”);
  if (Serial1.available() > 0) {
      String Data=Serial1.readString();
      Serial.println(Data);
      Serial.println(“recieved”);
  }
}
