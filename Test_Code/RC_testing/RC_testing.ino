
#include <SD.h>
#include <Servo.h>
#include <SPI.h>
#include <Wire.h>



void setup() {
  Serial.begin(9600);
  Serial1.begin(9600);
}




void loop() {
 
  //Serial.println("Running");
  if (Serial1.available() > 0) {
      String Data=Serial1.readString();
      Serial.println(Data);
      Serial.println("recieved");
    
      }




}
