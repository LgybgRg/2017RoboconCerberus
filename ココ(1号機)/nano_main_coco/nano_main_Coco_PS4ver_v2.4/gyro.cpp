#include<arduino.h>

extern int offset;
extern bool Serialflag;

//ジャイロセンサからの値を取得
int GetGyro() {
  int yaw = 0;
  static int i = 0;
  if (Serial.available() >= 3) {
    Serialflag = true;
    if (Serial.read() == 'H') {
      yaw = (Serial.read() << 8) | Serial.read();
      yaw -= offset;
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
int GetGyroOffset() {
  int yaw = 0;
  static int i = 0;
  if (Serial.available() >= 3) {
    Serialflag = true;
    if (Serial.read() == 'H') {
      yaw = (Serial.read() << 8) | Serial.read();
      i = yaw;
      return yaw;
    }
  }
  return i;
}



