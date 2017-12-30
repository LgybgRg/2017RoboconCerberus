#include<Arduino.h>

#include"i2cMasterWriter.h"
#include"ControllerManager.h"

extern int offset;

void Tire() {

  static bool counter_L, flag_L;
  static int gyro;
  //Omni_gyroCorrection();

  //auto_run();
  Omni.SetSingleData(0, DS4.GetLeftHatX());
  Omni.SetSingleData(1, DS4.GetLeftHatY());
  Omni.SetSingleData(2, DS4.GetRightHatX());
    
  Omni.SetSingleData(3, GetGyro() >> 8 & 0x00ff);
  Omni.SetSingleData(4, GetGyro() & 0x00ff);

  Omni.SetSingleData(5, !DS4.GetCrossToggle());
  Omni.SetSingleData(6, DS4.GetStartToggle());
}
