/*
 * Fish encoder + servo fins (transmitter code)
 * 
 * Transmits 4 signals: 
 *      servo: up-down, left-right
 *      tail: turnValue, power
 *      
 * Edited by: Tasnia, Brianna, Heidi, Allison
 * 
 */

int sensorValueX = analogRead(A0);    //declare Funduino joystick
int sensorValueY = analogRead(A1);
int ascii_sumX, ascii_sumY, ascii_sumT, ascii_sumP;     //checkSum values
int xMap, yMap; 
int tMap = 5;         //mapped values of turnVal
int pMap = 5;         //mapped values of power
String xstr, ystr, pstr, tstr;
int timer1 = 0;       //store the time when reading values from Funduino
int timer2 = 0;       //store the time when sending T signals
int timer3 = 0;       //store the time when pressing left-right buttons
int timer4 = 0;       //store the time when pressing up-down buttons
int timer5 = 0;       //store the time when sending P signals
int bounceDelay = 350;
//signal1:R;     signal2:U;     signal3:T;      signal4:P
char signal1[20], signal2[20], signal3[20], signal4[20];
int leftbutton = 5;           //declare Funduino buttons
int rightbutton = 3;
int upbutton = 2;
int downbutton = 4;

void setup() {
  // Begin the Serial at 9600 Baud
  Serial.begin(9600);
  Serial1.begin(9600, SERIAL_8O1);
  pinMode(sensorValueX, INPUT);
  pinMode(sensorValueY, INPUT);
  pinMode(leftbutton,INPUT_PULLUP);
  pinMode(rightbutton,INPUT_PULLUP);
  pinMode(upbutton,INPUT_PULLUP);
  pinMode(downbutton,INPUT_PULLUP);
}

void loop() {
  if (millis() - timer1 > 200) {
    timer1=millis();
    sensorValueX = analogRead(A0);
    sensorValueY = analogRead(A1);
    xMap = map(sensorValueX, 0, 1023, 0, 9);
    yMap = map(sensorValueY, 0, 1023, 0, 9);
    
    if((digitalRead(leftbutton) == LOW) && tMap > 1) {
      if (millis()-timer3 > bounceDelay){
        tMap--;
        Serial.println("button low left");
        timer3 = millis();
      }
    }
    else if ((digitalRead(rightbutton)== LOW) && tMap < 9) {
      if (millis()-timer3 > bounceDelay){
        tMap++;
        Serial.println("button high right");
        timer3 = millis();
      }
    }
    
    if((digitalRead(downbutton) == LOW) && pMap > 0) {
      if (millis()-timer4 > bounceDelay){
        pMap--;
        Serial.println("button low down");
        timer4 = millis();
      }
    }
    else if ((digitalRead(upbutton)== LOW) && pMap < 9) {
      if (millis()-timer4 > bounceDelay){
        pMap++;
        Serial.println("button high up");
        timer4 = millis();
      }
    }
    
    ascii_sumX = 'R' + (xMap + '0');      //checkSum values
    ascii_sumY = 'U' + (yMap + '0');
    ascii_sumT = 'T' + (tMap + '0');
    ascii_sumP = 'P' + (pMap + '0');
    xstr = "R" + String(xMap) + String(ascii_sumX) + "?";
    ystr = "U" + String(yMap) + String(ascii_sumY) + "?";
    tstr = "T" + String(tMap) + String(ascii_sumT) + "?";    
    pstr = "P" + String(pMap) + String(ascii_sumP) + "?"; 
    strcpy(signal1, xstr.c_str());
    strcpy(signal2, ystr.c_str());
    strcpy(signal3, tstr.c_str());
    strcpy(signal4, pstr.c_str());
    if ((xMap>5 || xMap<4) && yMap<=5 && yMap>=4) { //if R is greater than 5 or less than 4 and U value is 4-5, only R signal is sent
          Serial1.write(signal1);
          Serial.println("Left-right signal:");
          Serial.println(signal1);
    }
    if (xMap<=5 && xMap>=4 && (yMap>5 || yMap<4)) { //if U is greater than 5 or less than 4 and R value is 4-5, only U signal is sent
          Serial1.write(signal2);
          Serial.println("Up-down signal:");
          Serial.println(signal2);
    }
    if (xMap<=5 && xMap>=4 && yMap<=5 && yMap>=4) { //If both R and U values are 4-5, only one signal is sent to bring fins back to original position
          Serial1.write(signal1);
          Serial.println("Original position signal:");
          Serial.println(signal1);
    }
    if (millis() - timer2 > 400){
      timer2 = millis();
      Serial1.write(signal3);
      Serial.print("Turn value: ");
      Serial.println(signal3);
    }
    if (millis() - timer5 > 400){
      timer5 = millis();
      Serial1.write(signal4);
      Serial.print("Power value: ");
      Serial.println(signal4);
    }
    //blocking function, no need to wait for delay if use flush
    Serial1.flush();
  }
}
