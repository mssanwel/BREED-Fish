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
int ascii_sum;     //checkSum value
int xMap, yMap;
int tMap = 5;         //mapped values of turnVal
int pMap = 5;         //mapped values of power
String sum_str;
int timer1 = 0;       //store the time when reading values from Funduino
int timer2 = 0;       //store the time when sending T signals
int timer3 = 0;       //store the time when pressing left-right buttons
int timer4 = 0;       //store the time when pressing up-down buttons
int timer5 = 0;       //store the time when sending P signals
int bounceDelay = 350;
//signal1:R;     signal2:U;     signal3:T;      signal4:P
char signal[20];
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
    ascii_sum = 'R' + (xMap + '0')+'U' + (yMap + '0')+'T' + (tMap + '0')+'P' + (pMap + '0');
    sum_str = "R" + String(xMap) +"U"+ String(yMap)+"T" + String(tMap) +"P"+ String(pMap) + String(ascii_sum) + "?";
    strcpy(signal,sum_str.c_str());
    Serial1.write(signal);
    Serial.print("Signal: ");
    Serial.println(signal);
    //blocking function, no need to wait for delay if use flush
    Serial1.flush();
  }
}
