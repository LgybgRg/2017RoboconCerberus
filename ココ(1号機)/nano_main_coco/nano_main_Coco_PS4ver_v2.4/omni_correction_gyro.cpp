#include<arduino.h>
#include"PidController.h"
#include"ControllerManager.h"
#include"i2cMasterWriter.h"
#include"sub.h"

extern bool Serialflag;    //ジャイロ通信スイッチ切り替え忘れ防止
extern int offset;
extern buttonData Counter;

void Omni_gyroCorrection() {
     
  static bool counter, flag;
  static int flag_counter;

  if (DS4.GetTouchToggle() == 0 && Serialflag == 1) {
    if (DS4.GetRightHatX() > 120 && DS4.GetRightHatX() < 130)
      counter = 1;
    else
      counter = 0;

    Counter.newData = counter;

    if (Counter.oldData == 0 && Counter.newData == 1)
      flag = 1;

    Counter.oldData = Counter.newData;

    if (counter == 1 && flag == 1)
      flag_counter++;

    if (flag_counter > 400) {
      offset = GetGyroOffset();
      flag = 0;
      flag_counter = 0;
    }

    if (counter == 1 && flag == 0) {
      OmniCorrection.Update(GetGyro(), 0);
      Omni.SetSingleData(3, map(OmniCorrection.GetPower(), -255, 255, 0, 255));
    }
    else {
      Omni.SetSingleData(3, DS4.GetRightHatX());
    }
  }
  else {
    Omni.SetSingleData(3, DS4.GetRightHatX());
  }
}
