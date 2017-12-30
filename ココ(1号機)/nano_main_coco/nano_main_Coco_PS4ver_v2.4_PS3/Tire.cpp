#include<Arduino.h>

#include"i2cMasterWriter.h"
#include"ControllerManager.h"

void Tire() {

  static bool counter_L, flag_L;

  Omni_gyroCorrection();

  auto_run();

  Omni.SetSingleData(4, !DS4.GetCrossToggle());
  Omni.SetSingleData(5, DS4.GetStartToggle());
}
