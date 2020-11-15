/*
 * Basic code written to test the basic functionality of 
 * (i)  Radio Control Modules (Serial based)
 * (ii) Simple PWM control of a motor
 * 
 * This is on the receiver side, which, upon receipt of a signal will control the motor
 * Receiver should be an Arduino Mega, as they have multiple Serial ports.
 * Written by Timothy Ng 15/10/2020
 * 
 */


//Arduino Pin Declarations
unsigned int pwm_Pin = 10;
unsigned int motor_Pwm = 0;

bool red_Flag = LOW;
bool on = LOW;
unsigned long int timer1 = 0;
unsigned long int timer2 = 0;

//Radio Signal 
char myStr = '0';

void setup() {
 // Define the Serial ports, with Serial1 as the Receiver Port for Data from the transmitter. 
 // The RC is defined as 8 bits, Odd parity, 1 check digit. Default Arduino setting is SERIAL_8N1, where N means no parity.
 Serial1.begin(9600,SERIAL_8O1);
 Serial.begin(9600);

 // Set the PWM frequency of Timer 2 to be 3kHz
 TCCR2B = TCCR2B & 0b11111000 | 0x01;

 //Define the output pins
 pinMode(pwm_Pin, OUTPUT);
}

void loop() {

//Checks if there is any information in the Serial Buffer
  
  if(Serial1.available() ){
    myStr = Serial1.read(); //receives a signal from 0-9
    Serial.println(myStr); 
    int x = myStr - '0' ;   // Converts the character into an integer
    motor_Pwm = map(x, 0,9,0,255); //maps the value received (0-9) to (0-255)
    timer1 = millis();
  }

  if (millis() - timer1 > 3000) {
    Serial.print("Motor Killed");
    motor_Pwm = 0; 
  }

  
  analogWrite (pwm_Pin, motor_Pwm);
  //Serial.println(motor_Pwm);
  
}
