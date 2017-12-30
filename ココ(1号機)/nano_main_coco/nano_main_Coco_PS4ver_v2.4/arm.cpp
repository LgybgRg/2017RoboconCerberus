#include<arduino.h>
#include"i2cMasterWriter.h"
#include"ControllerManager.h"

void arm() {
   
  if (DS4.GetR1Toggle()) {
    Arm.SetData(1, 255);
    Arm.SetData(4, -255);
  }
  else {
    Arm.SetData(1, 0);
    Arm.SetData(4, 0);
  }
  
}
