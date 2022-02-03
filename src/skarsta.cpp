#include <Arduino.h>
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

const int LONG_PRESS_TIME = 1000; // 1 seconds

int out1;
int out2;
volatile long position = 0;

void readEncoder();
void savePosition1();
void savePosition2();
void savePosition3();
void goUp();
void goDown();
void goTo1();
void stopMotor();

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
  
  upButton.OnPressed(goUp);
  upButton.OnUnPressed(stopMotor);
  downButton.OnPressed(goDown);
  downButton.OnUnPressed(stopMotor);
  save1Button.OnPressed(goTo1);
  save1Button.OnPressedForDuration(savePosition1,LONG_PRESS_TIME);
  // save2Button.OnPressedForDuration(savePosition2,LONG_PRESS_TIME);
  // save3Button.OnPressedForDuration(savePosition3,LONG_PRESS_TIME);
}

void callBack(){
  Serial.println("Called back");
}

void callBackForDuration(){
  Serial.println("callBackForDuration");
}

void loop() {
  upButton.Read();
  downButton.Read();
  save1Button.Read();
  save2Button.Read();
  save3Button.Read();
  resetButton.Read();
}

void writeLongIntoEEPROM(int address, long number)
{ 
  EEPROM.write(address, (number >> 24) & 0xFF);
  EEPROM.write(address + 1, (number >> 16) & 0xFF);
  EEPROM.write(address + 2, (number >> 8) & 0xFF);
  EEPROM.write(address + 3, number & 0xFF);
}

long readLongFromEEPROM(int address){
  return ((long)EEPROM.read(address) << 24) +
         ((long)EEPROM.read(address + 1) << 16) +
         ((long)EEPROM.read(address + 2) << 8) +
         (long)EEPROM.read(address + 3);
}

void readEncoder(){
  int b = digitalRead(ENCB);
  if(b > 0){
    position--;
  }
  else{
    position++;
  }
}

void savePosition1(){
  Serial.println("position: "+ String(position));
  writeLongIntoEEPROM(0, position);
}
// void savePosition2(){
//   EEPROM.write(1,position);
// }
// void savePosition3(){
//   EEPROM.write(2,position);
// }

void goTo1(){
  long target = readLongFromEEPROM(0);
  while(target!=position){
    Serial.println("position: "+ String(position));
    if(position<target){
      goUp();
    }
    else if(position>target){
      goDown();
    }
  }
}

void goUp(){
  // Serial.println("goUp");
  analogWrite(LPWM,0);
  analogWrite(RPWM,255);
}

void goDown(){
  // Serial.println("goDown");
  analogWrite(LPWM,255);
  analogWrite(RPWM,0);
}

void stopMotor(){
  Serial.println("stopMotor");
  analogWrite(RPWM,0);
  analogWrite(LPWM,0);
}
