#include<arduino.h>

#include"EncoderBoard.h"
#include"i2cMasterWriter.h"
#include"LoopCycleController.h"
#include"ControllerManager.h"
#include"sub.h"

void ALLUPDATE() {

  GetGyroOffset();
  enc_x.Update();
  enc_y.Update();
  DS4.Update();
  Omni.Update();
  Arm.Update();
  loopCtrl.Update();
  Solenoid.Update();

  Tire();
  solenoid();
  arm();

  //コントローラー切断時
  if (DS4.GetPS()) {
    Arm.Reset();
    Solenoid.Reset();
    Omni.Reset();
  }
}
