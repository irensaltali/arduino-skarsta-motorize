#include <Arduino.h>
#include <EEPROM.h>
#include <FreeButton.h>
#include "TM1637.h"

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
// #define RESET 12
#define DIO 12
#define CLK 13

// Buttons.
FreeButton upButton(UP);
FreeButton downButton(DOWN);
FreeButton save1Button(SAVE1);
FreeButton save2Button(SAVE2);
// FreeButton resetButton(RESET);
TM1637 tm1637(CLK,DIO);

//Motor Drive Current Analog Pins
#define RIS A3
#define LIS A1

const int LONG_PRESS_TIME = 1000; // 1 seconds

int out1;
int out2;
volatile long position = 0;
int8_t TimeDisp[] = {0x00,0x00,0x00,0x00};

void readEncoder();
void savePosition1();
void savePosition2();
void goUp();
void goDown();
void goTo1();
void goTo2();
void stopMotor();
void writeLongIntoEEPROM(int address, long number);

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

  writeLongIntoEEPROM(0, 0);
  writeLongIntoEEPROM(4, 0);
  
  upButton.OnPressed(goUp);
  upButton.OnUnPressed(stopMotor);
  downButton.OnPressed(goDown);
  downButton.OnUnPressed(stopMotor);
  save1Button.OnUnPressed(goTo1);
  save1Button.OnPressedForDuration(savePosition1,LONG_PRESS_TIME);
  save2Button.OnUnPressed(goTo2);
  save2Button.OnPressedForDuration(savePosition2,LONG_PRESS_TIME);


  tm1637.set();
  tm1637.init();
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
  // resetButton.Read();
  TimeDisp[0] = (position / 1000) % 10;
  TimeDisp[1] = (position / 100) % 10;
  TimeDisp[2] = (position / 10) % 10;
  TimeDisp[3] = (position / 1) % 10;
  tm1637.display(TimeDisp);
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
  Serial.println("position is saved for 1: "+ String(position));
  writeLongIntoEEPROM(0, position);
}
void savePosition2(){
  Serial.println("position is saved for 2: "+ String(position));
  writeLongIntoEEPROM(4, position);
}

void goTo(int q){
  long target = readLongFromEEPROM(q);
  Serial.println("target: "+ String(target));
  int direction =0;
  bool targetReached = false;
  Serial.println("position: "+ String(position));
  if(position<target && direction!=-1){
    direction=1;
    goUp();
  }
  else if(position>target && direction!=1){
    direction=-1;
    goDown();
  }
  else{
    stopMotor();
    targetReached = true;
  }

  while(!targetReached){
    delay(10);
    if(direction==1 && position>target){
      targetReached=true;
    }
    else if(direction==-1 && position<target){
      targetReached=true;
    }
  }
  stopMotor();
}

void goTo1(){
  goTo(0);
}
void goTo2(){
  goTo(4);
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
