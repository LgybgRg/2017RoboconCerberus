//2017/09/02

/*    include     */
#include"ControllerManager.h"
#include"i2cMasterWriter.h"
#include"EncoderBoard.h"
#include"LoopCycleController.h"
#include"PidController.h"
#include"sub.h"
#include<Wire.h>

/*    instance     */
ControllerManager DS4(9);
EncoderBoard enc(5);
i2cMasterWriter motorBoard(1, 8),Solenoid(3, 5),Mecanum(2, 6, 1);

static LoopCycleController loopCtrl(1000);     //ループ周期[us](ウデ)
static LoopCycleController loopMecanum(10000); //ループ周期[us](足)
static PidController pidCtrl(2.0, 0, 0.05, loopCtrl.Cycle_us(), 300.0);   //ウデPID
static PidController MecanumCorrection(4.5, 0, 0.5, loopMecanum.Cycle_us(), 300.0); //足PID
//比例係数 / 積分係数 / 微分係数 / ループ周期 / 積分量の最大値

//クリック処理用
struct button_data {
  bool oldData = 0;
  bool newData = 0;
};

/*    variable    */
int ReferenceValue = 0;
int offset;


void setup() {

  Wire.begin();
  Serial.begin(115200);
  Wire.setClock(400000UL); // Set I2C frequency to 400kHz

  enc.Init();            //基板の初期化(Wire.begin()の後に呼び出す)
  enc.SetPullup(EncoderBoard::PULLUP_ENABLE);
  enc.ResetCount();      //カウントのリセット

  pidCtrl.ClearPower();   //操作量の値をすべてクリア
  MecanumCorrection.ClearPower();
}

void loop() {

  AllUpdate();

  motor_board();
  Tire();
  solenoid();
  Gyro();


  //コントローラー切断時
  if (DS4.GetPS()) {
    Solenoid.Reset();
    motorBoard.Reset();
    Mecanum.Reset();
  }
}

/*    ジャイロセンサ     */
void Gyro() {

  GetGyroOffset();

  //DS4タッチパッド押下で零位置取得
  if (DS4.GetTouchClick())
    offset = GetGyroOffset();
  /*
    Serial.print(offset);
    Serial.print("\t");
    Serial.println(GetGyro());
  */
}

void AllUpdate() {
  enc.Update();
  DS4.Update();
  motorBoard.Update();
  Mecanum.Update();
  loopCtrl.Update();
  Solenoid.Update();
}

/*    motorBoardへ送信するデータ処理    */
void motor_board() {

  /*     剣振り上げ振り下ろし     */

  if (DS4.GetR2())
    motorBoard.SetData(1, 255);
  else if (DS4.GetL2())
    motorBoard
    .SetData(1, -255);
  else
    motorBoard.SetData(1, 0);

  /*        傾け       */

  //微調整

  if (DS4.GetSelect()) {
    if (DS4.GetL1())
      ReferenceValue++;
    else if (DS4.GetR1())
      ReferenceValue--;
  }
  //45°傾ける
  if (DS4.GetL1())
    ReferenceValue = 300;
  else if (DS4.GetR1())
    ReferenceValue = -300;
  //直立

  if (DS4.GetDown())
    ReferenceValue = 0;

  //PID
  pidCtrl.Update(enc.GetCount(), ReferenceValue);
  motorBoard.SetData(3, pidCtrl.GetPower());
}

/*    メカナム     */
void Tire() {
  //PID
  MecanumCorrection.Update(GetGyro(), 0);

  Mecanum.SetSingleData(0, DS4.GetLeftHatX());
  Mecanum.SetSingleData(1, DS4.GetLeftHatY());

  //↑ボタンでジャイロ補正のON/OFF
  if (DS4.GetR3())
    Mecanum.SetSingleData(2, map(MecanumCorrection.GetPower(), -255, 255, 0, 255));
  else
    Mecanum.SetSingleData(2, DS4.GetRightHatX());

  Mecanum.SetSingleData(3, DS4.GetL3());
  Mecanum.SetSingleData(4, 0);
  Mecanum.SetSingleData(5, DS4.GetStartToggle() == 0 ? 0 : 2);
}

/*    ソレノイド    */
void solenoid() {

  Solenoid.SetSingleData(0, DS4.GetL3Toggle());
  Solenoid.SetSingleData(1, DS4.GetTriangle());
  Solenoid.SetSingleData(2, DS4.GetCircle());
  Solenoid.SetSingleData(3, DS4.GetCross());
  Solenoid.SetSingleData(4, DS4.GetSquare());
}
