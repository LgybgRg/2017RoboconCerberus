#include "Omni.h"
#include"MovingAverage.h"
#include <Wire.h>

#define DEBUG

const int i2cAddress = 2;
const int AverageSize = 120;
const int MaxPower = 190;
const int CEASETIME_MS = 500;


int16_t leftHatX, leftHatY, rightHatX;
byte reverse;
bool l3, r3;
static int16_t Power[4];
int16_t tmp = 0;

unsigned long CeaseCounter;
bool flag;

Omni Mecanum4(MaxPower, AverageSize);

void setup() {
  delay(2000);
  Serial.begin(115200);

  Wire.begin(i2cAddress);
  Wire.onReceive(receiveEvent);
  Mecanum4.SetPWMFrequency();
}

void loop() {

  Mecanum4.SetAxis(leftHatX, leftHatY, rightHatX);
  Mecanum4.Set4WheelPower(Power);

  //減速モード
  if (l3 == 1 || r3 == 1) {
    for (int i = 0; i < 4; i++)
      Power[i] = Power[i] / 4;
  }
  /*
    //右を正面に
    if (startCount == 1) {
    tmp = Power[0];
    Power[0] = Power[1];
    Power[1] = Power[2];
    Power[2] = Power[3];
    Power[3] = tmp;
    }
  */
  
  //後ろを正面に
  if (reverse == 1) {
    tmp = Power[0];
    Power[0] = Power[2];
    Power[2] = tmp;
    tmp = Power[1];
    Power[1] = Power[3];
    Power[3] = tmp;
  }
/*
  //左を正面に
  else if (startCount == 3) {
    tmp = Power[3];
    Power[3] = Power[2];
    Power[2] = Power[1];
    Power[1] = Power[0];
    Power[0] = tmp;
  }
  */

  if (flag) {
    CeaseCounter = millis();
    flag = false;
  }
  else{
   if((millis()-CeaseCounter)>CEASETIME_MS){
      for(int i=0;i<4;i++){
         Power[i]=0;
      }
   }
  }
  
#ifdef DEBUG
  Serial.print(Power[0]); Serial.print("\t");
  Serial.print(Power[1]); Serial.print("\t");
  Serial.print(Power[2]); Serial.print("\t");
  Serial.println(Power[3]);
#endif
  Mecanum4.Drive(Power);
}

void receiveEvent(int howMany) {
  while (Wire.available()) {
   flag=true;
    leftHatX  = Wire.read();
    leftHatY  = Wire.read();
    rightHatX  = Wire.read();
    l3 = Wire.read();
    r3 = Wire.read();
    reverse = Wire.read();
  }
}

