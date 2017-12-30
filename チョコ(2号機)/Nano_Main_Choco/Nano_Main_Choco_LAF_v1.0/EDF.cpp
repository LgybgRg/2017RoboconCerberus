#include<arduino.h>
#include"ControllerManager.h"
#include"i2cMasterWriter.h"

void edf() {
  if (DS4.GetCircleToggle())
    EDF.SetSingleData(0, 180);
  else
    EDF.SetSingleData(0, 0);
}

