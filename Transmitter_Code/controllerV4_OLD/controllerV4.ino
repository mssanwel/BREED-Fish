#include <Adafruit_SSD1331.h>

/*Fish remote controller
   Complete on 25/01/2019 Version 4.0
  By Shen Zhong
*/

#include <Adafruit_SSD1306.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SPITFT.h>
#include <Adafruit_SPITFT_Macros.h>
#include <gfxfont.h>
#include <SPI.h>
#include <Wire.h>

#define OLED_RESET 4
Adafruit_SSD1306 display(OLED_RESET);

#define NUMFLAKES 10
#define XPOS 0
#define YPOS 1
#define DELTAY 2
#define LOGO16_GLCD_HEIGHT 16
#define LOGO16_GLCD_WIDTH  16
static const unsigned char PROGMEM logo16_glcd_bmp[] =
{ B00000000, B11000000,
  B00000001, B11000000,
  B00000001, B11000000,
  B00000011, B11100000,
  B11110011, B11100000,
  B11111110, B11111000,
  B01111110, B11111111,
  B00110011, B10011111,
  B00011111, B11111100,
  B00001101, B01110000,
  B00011011, B10100000,
  B00111111, B11100000,
  B00111111, B11110000,
  B01111100, B11110000,
  B01110000, B01110000,
  B00000000, B00110000
};
#if (SSD1306_LCDHEIGHT != 32)
#error("Height incorrect, please fix Adafruit_SSD1306.h!");
#endif

const int XaxisPin   = A2; //direction
const int YaxisPin   = A7; //speed
const int ResistorPin = A6;


int xaxis   = 0;
int yaxis   = 0;
int resistor = 0;
int button1  = 0; //max button
int button2  = 0; //stop button
int button3  = 0;
int button4  = 0;

char inComingbyte[8];
char sendData[8];
int controllerState   = 0;
long lastReciveTime   = 0;
int dutyCycleLimit     = 0;
int sendSpeed         = 0;
int userDutyCycle     = 0;
char sendTurn          = 0;
int receivedDutyCycle = 0;
int receivedDutyCycleLimit = 0;
char receivedTurn      = 0;
int Increment   = 5;
char dutyC[3];
char speedC[3];
char turnC[3];
unsigned int right_count = 0;
unsigned int left_count  = 0;


void setup() {
  // put your setup code here, to run once:
  Serial.begin(19200, SERIAL_8O1);
  //Serial.setTimeout(200);
  pinMode( 5, INPUT); //b1
  pinMode( 4, INPUT); //b2
  pinMode( 3, INPUT); //b3
  pinMode( 2, INPUT); //b4
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);  // initialize with the I2C addr 0x3C (for the 128x32)
  display.display();
  display.clearDisplay();
}

void loop() {

  informationdisplay();
  getUserInput();
  //handshake();
  Serial.println(analogRead(A7));
  //Serial.println(inComingbyte);
}
void informationdisplay(void) {
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0, 0);
  //display.println(sendData);
  //display.println(inComingbyte);
  display.print("S: ");
  display.print(sendSpeed);
  display.print(" Max: ");
  display.println(dutyCycleLimit);
  display.print("R: ");
  display.print(receivedDutyCycle);
  display.print(" Max: ");
  display.println(receivedDutyCycleLimit);
  display.println(xaxis);
  display.println(analogRead(YaxisPin));

  /*if(receivedTurn == 'r'){
    right_count--;
    }
    else if(receivedTurn == 'l'){
    left_count--;
    }
    display.print("right:");
    display.print(right_count);
    display.print(" left:");
    display.println(left_count);
  */


  if (receivedTurn == 'r') {
    display.println("Turn Right");
  }
  else if (receivedTurn == 'l') {
    display.println("Turn Left");
  }

  display.display();
  display.clearDisplay();
}

void getUserInput() {
  /* This function collects the user input and translates it to
     duty cycle, duty cycle limits, and turn information that will
     then be relayed to the handshak-protocol to be sent out.
  */
  //Note that this is a polling way of doing it, which is find since it's a small code.
  button1 = digitalRead(5);
  button2 = digitalRead(4);
  //button3 = digitalRead(3);
  //button4 = digitalRead(2);
  xaxis = analogRead(XaxisPin);

  resistor = analogRead(ResistorPin);
  userDutyCycle = analogRead(YaxisPin);
  // Duty Cycle Controls

  if (userDutyCycle > dutyCycleLimit) userDutyCycle = dutyCycleLimit;
  if (userDutyCycle < 0) userDutyCycle = 0;
  if (button2 == 1) { //stop
    sendSpeed = 0;
    userDutyCycle = 0;
  }

  // Turning Controls
  if (xaxis > 600) {
    right_count++;                                      //turn right
    sendTurn = 'r';
  }
  else if (xaxis < 400) {
    left_count++;
    sendTurn = 'l';                                     //turn left
  }
  else sendTurn = '0';

  dutyCycleLimit = map(resistor, 0, 1023, 0, 99);

  if (button1 == 1) sendSpeed = dutyCycleLimit;
  else sendSpeed = userDutyCycle;

  // Need to test if all the buttons are working.... and label them.

}

void handshake() {
  if (controllerState == 0) {                          //sending signal
    sprintf(sendData, "c%02u%02u%ce", dutyCycleLimit, sendSpeed, sendTurn);
    Serial.print(sendData);
    controllerState = 1;
    lastReciveTime = millis();
  }
  if (controllerState == 1) {                          //reciving signal
    while (Serial.available()) {
      Serial.readBytesUntil('e', inComingbyte, 7);     //change controllerState if start bit is 'b'
      if (inComingbyte[0] == 'f') {
        controllerState = 0;
        for (int i = 0; i < 2; i++) {
          dutyC[i] = inComingbyte[i + 1];
          speedC[i]  = inComingbyte[i + 3];
        }
        receivedDutyCycleLimit = atoi(dutyC);
        receivedDutyCycle     = atoi(speedC);
        receivedTurn      = inComingbyte[5];
      }
      //Serial.read();
    }
    if (millis() - lastReciveTime > 200) {              //send again if waiting time exceeds 200ms
      controllerState = 0;
    }
  }
}
