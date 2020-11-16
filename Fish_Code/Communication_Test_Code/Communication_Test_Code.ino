//Arduino Pin Declarations
unsigned int pwm_Pin = 10;//10;
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
long int tailDelay1=2000;
//long int tailTimer2=millis();
//long int tailDelay2=2000;
int encoderPin0   =  29;
int offset=0;
int turningDiff=0.5;

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

  //power = 8;//myStr - '0' ; // This line
  //turnVal=4;
  encoder();
  delay(1000);
  //offset=encoderVal;
  Serial.print("Offset= ");
  Serial.println(offset);
}

void loop() {

  //Checks if there is any information in the Serial Buffer
  if(Serial1.available() ){
    myStr = Serial1.read(); //receives a signal from 0-9
    //Add code to seperate myStr to appropriate sections and store in variaibles below
    Serial.println(myStr); 
    // Converts the character into an integer

    if (myStr>='A' and myStr<='I'){
      turnVal=myStr-'A'+1; // and this line
    }
    if (myStr>='0' and myStr<='9')
      power = myStr - '0' ; // This line
     
    timer1 = millis();

    //turnDiff
    if (turnVal!=5){
      turningDiff = map((turnVal%5), 1,4,0.5,0.1);
    }
    
    if ((millis()-tailTimer1)%4000>tailDelay1){
      motor_Pwm = map(power, 0,9,0,255);
    }
  }
  
  encoder();
  
  //Kill switch
  if (millis() - timer1 > 3000) {
    motor_Pwm = 0; 
  }
  else{
  //{
    // Turning control Left
    if ((turnVal>=1) and (turnVal<=4)){
      Serial.println("Turning Left");
      if ((abs(encoderVal-offset)>=206) and (abs(encoderVal-offset)<=306)){
        if ((millis()-tailTimer1)%4000<tailDelay1){
            motor_Pwm = map(power, 0,9,0,255)*turningDiff;
        }
        else{
          motor_Pwm = map(power, 0,9,0,255);
        }
      }
    }
    
    // Going Straight
    else if (turnVal==5){
      Serial.println("Going straight");
      motor_Pwm = map(power, 0,9,0,255); //maps the value received (0-9) to (0-255)
    }
    
    // Turning control Left
    else if ((turnVal>=6) and (turnVal<=9)){
      Serial.println("Turning Right");
      if ((abs(encoderVal-offset)>=718) and (abs(encoderVal-offset)<=818)){
        if ((millis()-tailTimer1)%4000<tailDelay1){
            motor_Pwm = map(power, 0,9,0,255)*turningDiff;
        }
        else{
          motor_Pwm = map(power, 0,9,0,255);
        }
      } 
    }
  
  }
  analogWrite (pwm_Pin, motor_Pwm);
  Serial.print("motor pwm:                    ");
  Serial.println(motor_Pwm);
  //delay(500);
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
  
  //float angularV;
  //angularVelocity[0]= angularVelocity[1];
  //angularVelocity[1] = angularVelocity[2];
  //angularVelocity[2] = encoderValueRaw;
    
  //if(runtime != 0){
  //  angularV = (angularVelocity[2] - angularVelocity[0])*2*PI/(2*runtime*1024/1000/1000);  //gives the angular velocity in rad/s.
  //}
  //Serial.print(angularV);
  //Serial.print("\t");
  //Serial.println(encoderValueRaw-600);
  
  //if((counter%10) == 0 ) analogWrite(5, encoderValueRaw*255/1024);
  //if(counter == 1000){
    //runtime = (micros() - timer)/1000;
    //Serial.print("the average time for the loop is ");
    //Serial.print(runtime);
    //Serial.println(" microseconds");
    
    //counter = 0;  //reset the counter
    //timer = micros();  //reset the timer base
  //}
  //counter++;
  //encoderCounter++;
  //delay(500);
  
  encoderVal= encoderValueRaw;
}
