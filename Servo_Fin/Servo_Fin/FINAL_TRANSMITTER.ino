int sensorValueX = analogRead(A2);
int sensorValueY = analogRead(A3);
int ascii_sumX;
int ascii_sumY;
int xMap, yMap;
String xstr, ystr, powstr;
int timer1 = 0;

char signal1[20], signal2[20];
void setup() {
  // Begin the Serial at 9600 Baud
  Serial.begin(9600);
  Serial1.begin(9600);
  pinMode(sensorValueX, INPUT);
  pinMode(sensorValueY, INPUT);
}
void loop() {
  if (millis() - timer1 > 200) {
    timer1=millis();
    sensorValueX = analogRead(A2);
    sensorValueY = analogRead(A3);
    xMap = map(sensorValueX, 0, 1023, 0, 9);
    yMap = map(sensorValueY, 0, 1023, 0, 9);
    xstr = "x" + String(xMap);
    ystr = "y" + String(yMap);
    xMap = map(sensorValueX, 0, 1023, 0, 9);
    yMap = map(sensorValueY, 0, 1023, 0, 9);
    ascii_sumX = 84 + (xMap + '0');
    ascii_sumY = 85 + (yMap + '0');
    xstr = "T" + String(xMap) + String(ascii_sumX) + "?" ;
    ystr = "U" + String(yMap) + String(ascii_sumY) + "?";
    strcpy(signal1, xstr.c_str());
    strcpy(signal2, ystr.c_str());
    Serial1.write(signal1);
    //Serial1.write(signal2);
    Serial.println(signal1);
    Serial.println(signal2);

    //blocking function, no need to wait for delay if use flush
    Serial1.flush();
  }

}
