#include<arduino.h>

#include"ControllerManager.h"
#include"PidController.h"
#include"i2cMasterWriter.h"
#include"EncoderBoard.h"

int GetAngle_deg(long origin_x, long origin_y, long ref_x, long ref_y) {

  double angle = atan2((ref_y - origin_y), (ref_x - origin_x));

  angle = round(degrees(angle));

  if (angle < 0) angle += 360;
  if (angle > 359) angle -= 360;

  return angle;
}

long GetDistance(long origin_x, long origin_y, long ref_x, long ref_y) {

  long distance = sqrt(pow(ref_y - origin_y, 2) + pow(ref_x - origin_x, 2));

  return distance;
}

void auto_run() {

  long reference_x, reference_y;
  static int16_t Pid_analogPower;
  static long old_enc_x, old_enc_y;

  if (DS4.GetSquareClick()) {
    enc_x.ResetCount();
    enc_y.ResetCount();
  }

  if (DS4.GetL2() == 0 && DS4.GetR2() == 0) {
    Omni.SetSingleData(0, DS4.Getr());
    Omni.SetSingleData(1, DS4.Gettheta() >> 8 & 0x00ff);
    Omni.SetSingleData(2, DS4.Gettheta() & 0x00ff);
  }
  else {
    if (DS4.GetR2()) {
      reference_x = -10026;
      reference_y = 0;
      Pid_analogPower = DS4.GetAnalogR2();
    }
    else if (DS4.GetL2()) {
      reference_x = -35500;
      reference_y = 50000;
      Pid_analogPower = DS4.GetAnalogL2();
    }

    //目標地点との距離，角度[deg]を計算
    int deg_auto = GetAngle_deg(reference_x, reference_y, enc_x.GetCount(), enc_y.GetCount());
    long distance = GetDistance(reference_x, reference_y, enc_x.GetCount(), enc_y.GetCount());

    AutoRun.Update(distance, 0, Pid_analogPower);

    Omni.SetSingleData(0, abs(AutoRun.GetPower()));
    Omni.SetSingleData(1, deg_auto >> 8 & 0x00ff);
    Omni.SetSingleData(2, deg_auto & 0x00ff);
  }
}
