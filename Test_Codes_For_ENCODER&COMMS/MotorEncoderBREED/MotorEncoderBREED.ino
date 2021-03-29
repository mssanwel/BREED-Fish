long int counter =0;
long int runtime = 0;
long int timer = 0;
float timeStep = 0;
long int encoderCounter = 0;
double angularVelocity[3] = {0,0,0};


void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  Serial.println("Serial Port Initialized!");
  for(int encoderPin = 22;encoderPin <= 31;encoderPin ++){   //absolute encoder pin setup
    pinMode(encoderPin,INPUT_PULLUP) ; 
  }
  Serial.println("Encoder Pins Initialized!");
  Serial.println("Setup is complete! Click to begin the program....");
  while(!Serial.read());
  timer = micros();
}


void loop() {
  // put your main code here, to run repeatedly:
  int x = PINA; 
  int y = PINB;
  int a[10];
  int b[10];
  for(int n = 0;n<10;n++){
    a[n] = !digitalRead(22+n);
  }
  b[9] = a[9];
  for(int i = 1;i<10;i++){
    b[9-i] = b[9-i+1]^a[9-i];  // xor
  }
  float encoderValueRaw = 512.0*b[9]+256.0*b[8]+128.0*b[7]+64.0*b[6]+32.0*b[5]+16.0*b[4]+8.0*b[3]+4.0*b[2]+2.0*b[1]+b[0];
  Serial.println(encoderValueRaw);
  float angularV;
  angularVelocity[0]= angularVelocity[1];
  angularVelocity[1] = angularVelocity[2];
  angularVelocity[2] = encoderValueRaw;
    
  if(runtime != 0){
    angularV = (angularVelocity[2] - angularVelocity[0])*2*PI/(2*runtime*1024/1000/1000);  //gives the angular velocity in rad/s.
  }
  Serial.print(angularV);
  Serial.print("\t");
  Serial.println(encoderValueRaw-600);
  
  if((counter%10) == 0 ) analogWrite(5, encoderValueRaw*255/1024);
  if(counter == 1000){
    runtime = (micros() - timer)/1000;
    //Serial.print("the average time for the loop is ");
    //Serial.print(runtime);
    //Serial.println(" microseconds");
    
    counter = 0;  //reset the counter
    timer = micros();  //reset the timer base
  }
  counter++;
  encoderCounter++;
  delay(500);
}
