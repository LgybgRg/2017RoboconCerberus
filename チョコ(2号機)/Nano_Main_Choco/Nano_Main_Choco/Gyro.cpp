#pragma once
#include<arduino.h>
extern int ofset;

//ジャイロセンサからの値を取得
int GetGyro() {
  int yaw = 0;
  static int i = 0;
  if (Serial.available() >= 3) {
    if (Serial.read() == 'H') {
      yaw = (Serial.read() << 8) | Serial.read();
      yaw -= ofset;
      if (yaw > 180)
        yaw -= 360;
      else if (yaw < -180)
        yaw += 360;
      i = yaw;
      return yaw;
    }
  }
  return i;
}


//初期位置を設定
int GetGyroOfset() {
  int yaw = 0;
  static int i = 0;
  if (Serial.available() >= 3) {
    if (Serial.read() == 'H') {
      yaw = (Serial.read() << 8) | Serial.read();
      i = yaw;
      return yaw;
    }
  }
  return i;
}

