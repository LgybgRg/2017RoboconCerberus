//[revive] 2017/09/15 低速モードの処理をOmni.hに移植

#include "Omni.h"
#include"MovingAverage.h"
#include <Wire.h>

const int I2C_ADDRESS = 2;
const int AVERAGE_SIZE = 100;
const int MAX_POWER = 250;
const int CEASETIME_MS = 500;

int16_t leftHatX, leftHatY, rightHatX;
byte startCount;
bool l3, r3;
static int16_t Power[4];
int16_t hoge = 0;

bool flag;
unsigned long CeaseCounter;

Omni Mecanum4(MAX_POWER, AVERAGE_SIZE);

#define DEBUG

void setup() {
  delay(2500);
  Serial.begin(115200);

  Wire.begin(I2C_ADDRESS);
  Wire.onReceive(receiveEvent);
  Mecanum4.SetPWMFrequency();
}

void loop() {

  Mecanum4.SetAxis(leftHatX, leftHatY, rightHatX, l3);
  Mecanum4.Set4WheelPower(Power);

  //減速モード
  if (l3 == 1 || r3 == 1) {
    for (int i = 0; i < 4; i++)
      Power[i] = Power[i] / 4;
  }

  //反転モード
  if (startCount == 1) {
    hoge = Power[0];
    Power[0] = Power[2];
    Power[2] = hoge;
    hoge = Power[1];
    Power[1] = Power[3];
    Power[3] = hoge;
  }
  
  if (flag == 0) {
    if ((millis() - CeaseCounter) > CEASETIME_MS) {
      for (int i = 0; i < 4; i++)
        Power[i] = 0;
    }
  }
  else {
    CeaseCounter = millis();
    flag = false;
  }

#ifdef DEBUG
  Serial.print(Power[0]); Serial.print("\t");
  Serial.print(Power[1]); Serial.print("\t");
  Serial.print(Power[2]); Serial.print("\t");
  Serial.println(Power[3]);
#endif

  //show();
  Mecanum4.Drive(Power);
}

void receiveEvent(int howMany) {

  while (Wire.available() > 5) {

    flag = true;

    leftHatX  = Wire.read();
    leftHatY  = Wire.read();
    rightHatX  = Wire.read();
    l3 = Wire.read();
    r3 = Wire.read();
    startCount = Wire.read();
  }

}

void show() {
  Serial.print(leftHatX); Serial.print("\t");
  Serial.print(leftHatY); Serial.print("\t");
  Serial.println(rightHatX);
}

