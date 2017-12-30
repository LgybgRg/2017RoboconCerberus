//[add]2017/09/11 エンコーダ実装
//[add]2017/09/14 刀部分のモータを追加

/*    include     */
#include"ControllerManager.h"
#include"EncoderBoard.h"
#include"LoopCycleController.h"
#include"PidController.h"
#include"i2cMasterWriter.h"
#include"sub.h"
#include<Wire.h>

/*    instance     */
ControllerManager DS4(9);
i2cMasterWriter Arm(1, 4), Solenoid(3, 5), Mecanum(2, 5, MECANUM), EDF(4, 1);
EncoderBoard SecondJoint(20); //第二関節エンコーダ

static LoopCycleController loopMecanum(10000);     //足回り
static LoopCycleController loopArm(1000);          //第二関節

static PidController MecanumCorrection(3.6, 0, 1.1, loopMecanum.Cycle_us(), 300.0);
static PidController pidJoint(2.0, 0, 0.1, loopArm.Cycle_us(), 300.0);

//比例係数 / 積分係数 / 微分係数 / ループ周期 / 積分量の最大値

//クリック処理用
struct button_data {
  bool oldData = 0;
  bool newData = 0;
};

button_data Counter;

/*    variable    */
int offset = 0;

//ジャイロ補正用変数
static int counter;
int flag;
static int flag_counter;

//ジャイロ基板からの通信
bool Serialflag;

//第二関節目標値
static int ReferenceValue;

void setup() {
  delay(2000);
  Wire.begin();
  Serial.begin(115200);
  Wire.setClock(400000UL); // Set I2C frequency to 400kHz

  MecanumCorrection.ClearPower();//操作量の値をすべてクリア
  pidJoint.ClearPower();

  SecondJoint.Init();            //基板の初期化(Wire.begin()の後に呼び出す)
  SecondJoint.SetPullup(EncoderBoard::PULLUP_ENABLE);
  SecondJoint.ResetCount();      //カウントのリセット

}
void loop() {
  AllUpdate();

  arm();
  Tire();
  edf();

  //コントローラー切断時
  if (DS4.GetPS()) {
    Solenoid.Reset();
    Mecanum.Reset(MECANUM);
    EDF.Reset();
  }
  //DataLogger();
}

void AllUpdate() {
  GetGyroOffset();
  DS4.Update();
  Arm.Update();
  Mecanum.Update();
  Solenoid.Update();
  EDF.Update();
  SecondJoint.Update();
  loopArm.Update();
}
//ウデ
void arm() {
  /*    LAP方式    */
  /*
      第二関節
      R2 / L2で目標値を変更
  */

  if (DS4.GetR2() == 0 && DS4.GetL2() == 0) {

    pidJoint.Update(SecondJoint.GetCount(), ReferenceValue);
    //LAP方式 -> 0～255に丸める
    Arm.toG2MotorDriverLAP(1, map(pidJoint.GetPower(), -255, 255, 255, 0));
  }
  else if (DS4.GetR2()) {
    Arm.toG2MotorDriverLAP(1, map(DS4.GetAnalogR2(),0,255,127,255));
    ReferenceValue = SecondJoint.GetCount();
  }
  else if (DS4.GetL2()) {
    Arm.toG2MotorDriverLAP(1, map(DS4.GetAnalogL2(),0,255,127,1));
    ReferenceValue = SecondJoint.GetCount();
  }

   Arm.Show();
  /*
    if (DS4.GetR2())
    ReferenceValue -= 2;

    else if (DS4.GetL2())
    ReferenceValue += 2;

    /*  エンコーダの回転数 を 0～1850 の範囲に収める  */

  /*if (ReferenceValue < 0)
    ReferenceValue = 0;
    else if (ReferenceValue > 1850)
    ReferenceValue = 1850;

    pidJoint.Update(SecondJoint.GetCount(), ReferenceValue);
    //LAP方式 -> 0～255に丸める
    Arm.toG2MotorDriverLAP(1, map(pidJoint.GetPower(), -255, 255, 255, 0));
  */

  //第一関節
  if (DS4.GetL1())
    Arm.toG2MotorDriverLAP(3, 1);
  else if (DS4.GetR1())
    Arm.toG2MotorDriverLAP(3, 255);
  else
    Arm.toG2MotorDriverLAP(3, 127);

  //サブアーム
  if (DS4.GetTriangle())
    Arm.toG2MotorDriverLAP(2, 40);
  else if (DS4.GetSquare())
    Arm.toG2MotorDriverLAP(2, 214);
  else
    Arm.toG2MotorDriverLAP(2, 127);
}

//足周り
void Tire() {

  if (DS4.GetTouchToggle() == 0 && Serialflag == 1) {       //タッチパッドが押されていない && データが受信できている
    if (DS4.GetRightHatX() > 120 && DS4.GetRightHatX() < 130)
      counter = 1;
    else
      counter = 0;

    //スティックの立ち上がり検出
    Counter.newData = counter;

    if (Counter.oldData == 0 && Counter.newData == 1)
      flag = 1;

    Counter.oldData = Counter.newData;

    //スティックが中央にあり，立ち上がっている場合
    if (counter == 1 && flag == 1)
      flag_counter++;

    //一定時間経過後，オフセット取得
    if (flag_counter > 500) {
      offset = GetGyroOffset();
      flag = 0;
      flag_counter = 0;
    }

    if (counter == 1 && flag == 0) {
      MecanumCorrection.Update(GetGyro(), 0);
      Mecanum.SetSingleData(2, map(MecanumCorrection.GetPower(), -255, 255, 0, 255));
    }
    else
      Mecanum.SetSingleData(2, DS4.GetRightHatX());
  }

  else
    Mecanum.SetSingleData(2, DS4.GetRightHatX());

  if (DS4.GetLeft())
    Mecanum.SetSingleData(0, 0);
  else if (DS4.GetRight())
    Mecanum.SetSingleData(0, 255);
  else
    Mecanum.SetSingleData(0, DS4.GetLeftHatX());
  if (DS4.GetUp())
    Mecanum.SetSingleData(1, 0);
  else if (DS4.GetDown())
    Mecanum.SetSingleData(1, 255);
  else
    Mecanum.SetSingleData(1, DS4.GetLeftHatY());

  Mecanum.SetSingleData(3, !DS4.GetCrossToggle()); //減速モード
  Mecanum.SetSingleData(4, DS4.GetStartToggle());  //反転モード
}
