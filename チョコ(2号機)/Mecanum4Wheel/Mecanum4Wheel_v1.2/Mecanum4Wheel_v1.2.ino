//2017/09/15 低速モードの処理をOmni.hに移植
//2017/09/18 反転モードの処理をOmni.hに移植
//2017/09/20 モータ出力値の導出方法に極座標方式を追加

#include "Omni.h"
#include"MovingAverage.h"
#include <Wire.h>

const int I2C_ADDRESS = 2;
const int AVERAGE_SIZE = 100;
const int MAX_POWER = 250;
const int CEASETIME_MS = 500;

int16_t leftHatX,leftHatY,rightHatX;
static int16_t Power[4];
bool lowSpeed_mode, reverse_mode;
int r, theta;

bool flag;
unsigned long CeaseCounter;

Omni Omni4(MAX_POWER, AVERAGE_SIZE);

#define DEBUG

void setup() {
  delay(2500);
  Serial.begin(115200);

  Wire.begin(I2C_ADDRESS);
  Wire.onReceive(receiveEvent);
  Omni4.SetPWMFrequency();
}

void loop() {
  Omni4.SetAxis(leftHatX, leftHatY, rightHatX, lowSpeed_mode, reverse_mode);
  Omni4.Set4WheelPower(Power);

  /*  暴走防止用  */
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
  Omni4.Drive(Power);
}

void receiveEvent(int howMany) {

  while (Wire.available() > 4) {

    flag = true;

    leftHatX=Wire.read();
    leftHatY = Wire.read();
    rightHatX  = Wire.read();
    lowSpeed_mode = Wire.read();
    reverse_mode = Wire.read();
  }

}

void show() {
  Serial.print(leftHatX); Serial.print("\t");
  Serial.print(leftHatY); Serial.print("\t");
  Serial.print(rightHatX); Serial.print("\t");
  Serial.print(lowSpeed_mode); Serial.print("\t");
  Serial.println(reverse_mode);
}

