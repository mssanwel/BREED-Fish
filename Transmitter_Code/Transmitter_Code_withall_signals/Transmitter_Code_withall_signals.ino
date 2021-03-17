int sensorValueX = analogRead(A2);
int sensorValueY = analogRead(A3);
int ascii_sumX, ascii_sumY, ascii_sumT;
int xMap, yMap; 
int tMap = 5;
String xstr, ystr, powstr, tstr;
int timer1 = 0;
int timer2 = 0;
char signal1[20], signal2[20], signal3[20];
int leftbutton = 5;
int rightbutton = 3;
void setup() {
  // Begin the Serial at 9600 Baud
  Serial.begin(9600);
  Serial1.begin(9600);
  pinMode(sensorValueX, INPUT);
  pinMode(sensorValueY, INPUT);
  pinMode(leftbutton,INPUT_PULLUP);
  pinMode(rightbutton,INPUT_PULLUP);
}
void loop() {
  if (millis() - timer1 > 200) {
    timer1=millis();
    sensorValueX = analogRead(A2);
    sensorValueY = analogRead(A3);
    xMap = map(sensorValueX, 0, 1023, 0, 9);
    yMap = map(sensorValueY, 0, 1023, 0, 9);
    if((digitalRead(leftbutton) == LOW) && tMap > 0) {
      tMap--;
      Serial.println("button low left");
    }
    else if ((digitalRead(rightbutton)== LOW) && tMap < 9) {
      tMap++;
      Serial.println("button high right");
    }
    ascii_sumX = 'R' + (xMap + '0');
    ascii_sumY = 'U' + (yMap + '0');
    ascii_sumT = 'T' + (tMap + '0');
    xstr = "R" + String(xMap) + String(ascii_sumX) + "?";
    ystr = "U" + String(yMap) + String(ascii_sumY) + "?";
    tstr = "T" + String(tMap) + String(ascii_sumT) + "?";    
    strcpy(signal1, xstr.c_str());
    strcpy(signal2, ystr.c_str());
    strcpy(signal3, tstr.c_str());
    if ((xMap>5 || xMap<4) && yMap<=5 && yMap>=4) { //if R is greater than 5 or less than 4 and U value is 4-5, only T signal is sent
          Serial1.write(signal1);
          Serial.println("Left-right signal:");
          Serial.println(signal1);
    }
    if (xMap<=5 && xMap>=4 && (yMap>5 || yMap<4)) { //if U is greater than 5 or less than 4 and T value is 4-5, only U signal is sent
          Serial1.write(signal2);
          Serial.println("Up-down signal:");
          Serial.println(signal2);
    }
    if (xMap<=5 && xMap>=4 && yMap<=5 && yMap>=4) { //If both T and U values are 4-5, only one signal is sent to bring fins back to original position
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
    //blocking function, no need to wait for delay if use flush
    Serial1.flush();
  }
}
