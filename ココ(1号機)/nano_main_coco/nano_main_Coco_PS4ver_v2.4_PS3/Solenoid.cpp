#include<arduino.h>
#include"i2cMasterWriter.h"
#include"ControllerManager.h"

void solenoid() {
  static unsigned long circle_counter, elapsedTime_ms;
  static bool flag, isPulled;
  static int spinTime = 1200, burstTime = 400;

  Solenoid.SetSingleData(0, DS4.GetL3Toggle());    //起き上がるところ

  //単発
  if (flag == false) {

    Solenoid.SetSingleData(1, DS4.GetUp());
    Solenoid.SetSingleData(2, DS4.GetRight());
    Solenoid.SetSingleData(3, DS4.GetDown());
    Solenoid.SetSingleData(4, DS4.GetLeft());

    if (DS4.GetTriangle())
      Arm.SetData(3, -255);
    else
      Arm.SetData(3, 0);
  }

  //2点同時(オート)
  if (DS4.GetCircleClick()) {

    circle_counter = millis();
    flag = true;

  }
  elapsedTime_ms = millis() - circle_counter;

  //ピンが抜かれていない場合(最初のみ) [isPulled => false]
  if (flag == true && isPulled == false) {

    if (elapsedTime_ms > 0) {
      Arm.SetData(3, -255);

      Solenoid.SetSingleData(2, 1);
      Solenoid.SetSingleData(4, 1);
    }
    if (elapsedTime_ms > burstTime) {
      Arm.SetData(3, 0);

      Solenoid.SetSingleData(2, 0);
      Solenoid.SetSingleData(4, 0);

      Solenoid.SetSingleData(1, 1);
      Solenoid.SetSingleData(3, 1);
    }

    if (elapsedTime_ms > (burstTime * 2) ) {
      Solenoid.SetSingleData(1, 0);
      Solenoid.SetSingleData(3, 0);
      flag = false;
      isPulled = true;
    }
    /*
        if (elapsedTime_ms > 0)
          Arm.SetData(3, -255);

        if (elapsedTime_ms > spinTime) {
          Arm.SetData(3, 0);

          Solenoid.SetSingleData(1, 1);
          Solenoid.SetSingleData(3, 1);
        }

        if (elapsedTime_ms > spinTime + burstTime) {

          Solenoid.SetSingleData(1, 0);
          Solenoid.SetSingleData(3, 0);

          Solenoid.SetSingleData(2, 1);
          Solenoid.SetSingleData(4, 1);
        }

        if (elapsedTime_ms > spinTime + (burstTime * 2) ) {
          Solenoid.SetSingleData(2, 0);
          Solenoid.SetSingleData(4, 0);
          flag = false;
          isPulled = true;
        }
    */
  }
  //ピンが抜かれた後 [isPulled => true]
  else if (flag == true && isPulled == true) {

    if (elapsedTime_ms > 0) {
      Solenoid.SetSingleData(1, 1);
      Solenoid.SetSingleData(3, 1);
    }

    if (elapsedTime_ms > burstTime) {
      Solenoid.SetSingleData(1, 0);
      Solenoid.SetSingleData(3, 0);

      Solenoid.SetSingleData(2, 1);
      Solenoid.SetSingleData(4, 1);
    }

    if (elapsedTime_ms > (burstTime * 2)) {
      Solenoid.SetSingleData(2, 0);
      Solenoid.SetSingleData(4, 0);
      flag = false;
    }
  }

  //Solenoid.Show();
}
