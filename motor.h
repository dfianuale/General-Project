// libraries
#include <PID_v1.h>

// motor pins
#define MApin1 11
#define MApin2 12
#define MSpin 10
//#define PWM = 16;

//encoder pins
const int MAEN = 18;
const int MBEN = 17;

//variables
long baudrate = 9600;
volatile int pos = 0;
long prevt = 0;
float eprev = 0;
float eintegral = 0;

//int pwmVal = 255;
int dir = 1;

void motorSetup() {
  // put your setup code here, to run once:
  Serial.begin(baudrate);
  pinMode(MAEN,INPUT);
  pinMode(MBEN,INPUT);
  pinMode(MApin1,OUTPUT);
  pinMode(MApin2,OUTPUT);
  pinMode(MSpin, OUTPUT);
  digitalWrite(MSpin,HIGH);
  attachInterrupt(digitalPinToInterrupt(MAEN),readEncoder, CHANGE);
}

void readEncoder(){
  int b = digitalRead(MBEN);
  if(b>0){
    pos++;
  }
  else{
    pos--;
  }
}

void setMotor(int dir, int pwmVal, int Mpin1, int Mpin2){
  if(dir==1){
    analogWrite(Mpin1,pwmVal);
    analogWrite(Mpin2,0);
    Serial.println("right");
  }
  else if(dir== -1){
    analogWrite(Mpin1,0);
    analogWrite(Mpin2,pwmVal);
    Serial.println("left");
  }
  else{
    analogWrite(Mpin1, 0);
    analogWrite(Mpin2, 0);
    Serial.println("nothing");
  }
  Serial.println(dir);
}
