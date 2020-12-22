//Arduino Pin Declarations
unsigned int pwm_Pin = 9;//9;
unsigned int motor_Pwm = 0;

bool red_Flag = LOW;
bool on = LOW;
unsigned long int timer1 = 0;
unsigned long int timer2 = 0;

//Radio Signal 
char myStr = '0';

long int counter =0;
long int runtime = 0;
long int timer = 0;
float timeStep = 0;
long int encoderCounter = 0;
double angularVelocity[3] = {0,0,0};
int turnVal=0; //Stores commmand value for control left and right. values 1-4 are left, 5 is straight, 6-9 are right
int encoderVal=0; //stores value of encoder at any given time. Updates from interrupt.
int power=0; //Stores power value from controller
long int tailTimer1=millis();
long int tailDelay1=500;
//long int tailTimer2=millis();
//long int tailDelay2=2000;
int encoderPin0   =  29;
int offset=627;
float turningDiff=0.3;
int prevPower=-1;

void setup() {
 // Define the Serial ports, with Serial1 as the Receiver Port for Data from the transmitter. 
 // The RC is defined as 8 bits, Odd parity, 1 check digit. Default Arduino setting is SERIAL_8N1, where N means no parity.
 //Serial.begin(9600,SERIAL_8O1);
 //Serial.begin(9600);
 
 // Set the PWM frequency of Timer 2 to be 3kHz
 TCCR2B = TCCR2B & 0b11111000 | 0x01;

 //Define the output pins
 pinMode(pwm_Pin, OUTPUT);
 pinMode(13, OUTPUT);

  Serial1.begin(9600,SERIAL_8O1);
  Serial.begin(9600);
  //Serial.begin(115200);
  //Serial.println("Serial Port Initialized!");

 
  for(int encoderPin = encoderPin0; encoderPin <= 47;encoderPin = encoderPin + 2){   //absolute encoder pin setup
    pinMode(encoderPin,INPUT_PULLUP);
    Serial.println(encoderPin);
  }
  
  while(!Serial.read());
  timer = micros();

  //attachInterrupt(digitalPinToInterrupt(20), encoder, HIGH);
  

  Serial.println("Encoder Pins Initialized!");
  Serial.println("Setup is complete! Click to begin the program....");

  encoder();
  //offset=encoderVal;
  Serial.print("Offset= ");
  Serial.println(offset);
  turnVal=5;
  power=0;
}

void loop() {

  //Checks if there is any information in the Serial Buffer
  if(Serial1.available() ){
    myStr = Serial1.read(); //receives a signal from 0-9
    //Add code to seperate myStr to appropriate sections and store in variaibles below
    Serial.print("Recieved:                          => ");
    Serial.println(myStr); 
    // Converts the character into an integer

    if (myStr>='A' and myStr<='I'){
      turnVal= myStr - 'A'+1; // and this line
    }
    if (myStr>='0' and myStr<='9'){
      power = myStr - '0' ; // This line
    }

    /*//turnDiff
    if (turnVal != 5){
      turningDiff = map((turnVal%5), 1,4,5,1);
      Serial.print("Differntial:                                  ");
      turningDiff=turningDiff/10;
      Serial.println(turningDiff);
    
    }*/
    //serialFlush();
    timer1 = millis();

    //turnVal=;
    //power=9;

    
    //if (power==9){
      //prevPower=power;
      //analogWrite(pwm_Pin, 254);
      //power=0;
    //}
    
    /*if (prevPower!=power){
      prevPower=power;
      analogWrite(pwm_Pin, 254);
      //Serial.print("");
      Serial.println("                              Power updated");
    }*/
    //motor_Pwm = map(power, 0,9,0,255);
    
    /*if ((millis()-tailTimer1)>100){
      tailTimer1 = millis();
      motor_Pwm = map(power, 0,9,0,255);
      //Serial.print("");
      Serial.println("                              Power updated");
    }*/
  }

  //turnVal=5;
  //power=9;
  
  
  encoder();
  
  //Kill switch
  if (millis() - timer1 > 3000) {
    motor_Pwm = 0;
    analogWrite (pwm_Pin, motor_Pwm); 
    power=9;
  }
  else{
    // Turning control Left
    if ((turnVal>=1) and (turnVal<=4)){
      Serial.println("                                         Turning Left");
      if ((abs(encoderVal-offset)<=255) or (abs(encoderVal-offset)>= 756)){
            motor_Pwm = map(power, 0,9,0, 255)*turningDiff;
      }
      else{
        motor_Pwm = map(power, 0,9,0, 255);
      }
      //}
    }
    
    // Going Straight
    else if (turnVal==5){
      Serial.println("                                         Going straight");
      motor_Pwm = map(power, 0,9,0,255); //maps the value received (0-9) to (0-255)
    }
    
    // Turning control Right
    else if ((turnVal>=6) and (turnVal<=9)){
      Serial.println("                                         Turning Right");
      if ((abs(encoderVal-offset)<=255) or (abs(encoderVal-offset)>=756)){
        motor_Pwm = map(power, 0,9,0, 255)*turningDiff;
      }
      else{
        motor_Pwm = map(power, 0,9,0, 255);
      }
      
    }
  
  }
  //Removing analog write from being executed in each loop
  
  analogWrite (pwm_Pin, motor_Pwm);
  Serial.print("motor pwm:                    ");
  Serial.println(motor_Pwm);
  //analogWrite(pwm_Pin, 254);
  //delay(500);
}


void serialFlush(){
  while(Serial.available() > 0) {
    char t = Serial.read();
  }
}   





void encoder() {
  // put your main code here, to run repeatedly:
  //int x = PINA; 
  //int y = PINB;
  int a[10];
  int b[10];
  for(int n = 0 ; n<10 ; n++ ){
    a[n] = !digitalRead(encoderPin0 + 2*n);
  }
  b[9] = a[9];
  for(int i = 1;i<10;i++){
    b[9-i] = b[9-i+1]^a[9-i];  // xor
  }
  float encoderValueRaw = 512.0*b[9]+256.0*b[8]+128.0*b[7]+64.0*b[6]+32.0*b[5]+16.0*b[4]+8.0*b[3]+4.0*b[2]+2.0*b[1]+b[0];
  Serial.print("encodervalue : ");
  Serial.println(abs(encoderValueRaw-offset));
  
  
  encoderVal= encoderValueRaw;
}
