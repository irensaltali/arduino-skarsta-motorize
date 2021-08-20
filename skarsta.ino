#include <util/atomic.h> // For the ATOMIC_BLOCK macro
#include <EEPROM.h>
#include <EasyButton.h>

// Motor driver pins
#define ENCA 2 // YELLOW
#define ENCB 3 // WHITE
#define RPWM 4
#define LPWM 5
#define REN 6
#define LEN 7

//Button PINS
#define UP 8 //LEFT
#define DOWN 9 // RIGHT
#define SAVE1 10
#define SAVE2 11
#define SAVE3 12
#define RESET 13

#define RIS A3
#define LIS A1

const int SHORT_PRESS_TIME = 300; // 300 milliseconds
const int LONG_PRESS_TIME = 2000; // 2 seconds


int out1;
int out2;
long pre=-1;
int direct=0;
volatile long posi = 0; // specify posi as volatile: https://www.arduino.cc/reference/en/language/variables/variable-scope-qualifiers/volatile/
bool newOpen=false;

void setup() {
  Serial.begin(9600);
  pinMode(ENCA,INPUT);
  pinMode(ENCB,INPUT);
  pinMode(RPWM,OUTPUT);
  pinMode(LPWM,OUTPUT);
  pinMode(LEN,OUTPUT);
  pinMode(REN,OUTPUT);
  digitalWrite(REN,HIGH);
  digitalWrite(LEN,HIGH);
  attachInterrupt(digitalPinToInterrupt(ENCA),readEncoder,RISING);
  
  pinMode(UP,INPUT);
  pinMode(DOWN,INPUT);

  
  Serial.println("EEPROM.length():"+String(EEPROM.length()));
  newOpen=true;
}

void loop() {
int uu = digitalRead(UP);
if(uu==HIGH){
  uu = digitalRead(UP);
}

int dd = digitalRead(DOWN);
if(dd==HIGH){
  dd = digitalRead(DOWN);
}

if(digitalRead(SAVE1)==HIGH){
  Serial.println("SAVE1");
  savePos1();
}


long ris=  (analogRead(RIS)/ 1023.0)*5000;
long lis = (analogRead(LIS)/1023.0)*5000;

if(uu==1 && dd==1){
  Serial.println("DOUBLE");
  direct=0;
}
else if(uu==1){
  direct=1;
}
else if(dd==1){
  direct=-1;
}
else{
  direct=0;
}


  
  long pos = 0; 
  ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
    pos = posi;
  }

  
  byte pot =pos/2096;
  if(pre!=pot){
    Serial.println(pot);
    pre=pot; 
  }

  byte gotopos=0;
  EEPROM.get(0,gotopos);
  
  if(gotopos>0 && newOpen){
    Serial.println("gotopos:"+String(gotopos));
    goDown();
    gotopos--;
    EEPROM.write(0,gotopos);
    delay(300);
    if(gotopos==0)
      newOpen=false;
  }
  
  if(direct==1){
    goUp();
  }
  else if(direct==-1){
    goDown();
  }
  else{
    stopMotor();
  }
}

void readEncoder(){
  int b = digitalRead(ENCB);
  if(b > 0){
    posi++;
  }
  else{
    posi--;
  }
}

void savePos1(){
  byte a=3;
  EEPROM.write(0,a);
}

void goUp(){
  analogWrite(LPWM,0);
  analogWrite(RPWM,255);
}

void goDown(){
  analogWrite(LPWM,255);
  analogWrite(RPWM,0);
}

void stopMotor(){
  analogWrite(RPWM,0);
  analogWrite(LPWM,0);
}


// Button Reads
int UP_LAST_STATE=LOW;
unsigned long UP_PRESSED_TIME =0; 
int DOWN_LAST_STATE=LOW;
unsigned long DOWN_PRESSED_TIME =0;
int SAVE1_LAST_STATE=LOW;
unsigned long SAVE1_PRESSED_TIME =0;
int SAVE2_LAST_STATE=LOW;
unsigned long SAVE2_PRESSED_TIME =0;
int SAVE3_LAST_STATE=LOW;
unsigned long SAVE3_PRESSED_TIME =0;
int RESET_LAST_STATE=LOW;
unsigned long RESET_PRESSED_TIME =0;

String buttonRead(){
  int SAVE2_CURRENT_STATE = digiRead(SAVE2);

}


int digiRead(int buttonPin) {
  if(digitalRead(buttonPin)==HIGH)
    return digitalRead(buttonPin);
  return LOW;
}
