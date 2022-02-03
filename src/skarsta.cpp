#include <Arduino.h>
#include <util/atomic.h> // For the ATOMIC_BLOCK macro
#include <EEPROM.h>
#include <FreeButton.h>

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

void callBack();

// Buttons.
FreeButton upButton(UP);
FreeButton downButton(DOWN);
FreeButton save1Button(SAVE1);
FreeButton save2Button(SAVE2);
FreeButton save3Button(SAVE3);
FreeButton resetButton(RESET);

//Motor Drive Current Analog Pins
#define RIS A3
#define LIS A1

const int LONG_PRESS_TIME = 2000; // 2 seconds

int out1;
int out2;
long pre=-1;
int direct=0;
volatile long posi = 0;
bool newOpen=false;

void readEncoder();
void savePos1();
void goUp();
void goDown();
void stopMotor();
String buttonRead();
void onPressedForDuration();
void onPressed();
int digiRead(int buttonPin);

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
  

  save3Button.Read();
  // save3Button.OnPressedForDuration(onPressedForDuration, LONG_PRESS_TIME);
  resetButton.OnPressed(callBack);
  upButton.OnPressed(goUp);
  upButton.OnUnPressed(stopMotor);

  Serial.println("EEPROM.length():"+String(EEPROM.length()));
  newOpen=true;
}

void callBack(){
  Serial.println("Called back");
}

void loop() {
  buttonRead();
  upButton.Read();
  downButton.Read();
  save1Button.Read();
  save2Button.Read();
  save3Button.Read();
  resetButton.Read();
    
  
  if(digitalRead(SAVE1)==HIGH && false){
    Serial.println("SAVE1");
    savePos1();
  }
    
  long ris=  (analogRead(RIS)/ 1023.0)*5000;
  long lis = (analogRead(LIS)/1023.0)*5000;

  // Serial.println("ris:");
  // Serial.println(ris);
  // Serial.println("lis:");
  // Serial.println(lis);

  
  
  long pos = 0; 
  ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
    pos = posi;
  }



  byte gotopos=0;
  EEPROM.get(0,gotopos);
  
  if(gotopos>0 && newOpen && false){
    Serial.println("gotopos:"+String(gotopos));
    goDown();
    gotopos--;
    EEPROM.write(0,gotopos);
    delay(300);
    if(gotopos==0)
      newOpen=false;
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
  Serial.println("goUp");
  analogWrite(LPWM,0);
  analogWrite(RPWM,255);
}

void goDown(){
  Serial.println("goDown");
  analogWrite(LPWM,255);
  analogWrite(RPWM,0);
}

void stopMotor(){
  Serial.println("stopMotor");
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
  int SAVE3_CURRENT_STATE = digiRead(SAVE3);
  if(SAVE3_LAST_STATE==LOW && SAVE3_CURRENT_STATE==HIGH){
    SAVE3_PRESSED_TIME=millis();
    SAVE3_LAST_STATE=SAVE3_CURRENT_STATE;    
  }
  else if(SAVE3_LAST_STATE==HIGH && SAVE3_CURRENT_STATE==LOW){
    unsigned long dif= millis()-SAVE3_PRESSED_TIME;
    if(dif>LONG_PRESS_TIME)
      Serial.println("Long press SAVE3");
    Serial.println("dif:"+String(dif));
    SAVE3_LAST_STATE=SAVE3_CURRENT_STATE;
    SAVE3_PRESSED_TIME=0;
  }
  return " ";
}


int digiRead(int buttonPin) {
  if(digitalRead(buttonPin)==HIGH)
    return digitalRead(buttonPin);
  return LOW;
}
