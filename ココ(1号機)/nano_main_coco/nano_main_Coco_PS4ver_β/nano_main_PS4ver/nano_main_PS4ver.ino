//2017/08/30

/*    include     */
#include"ControllerManager.h"
#include"i2cMasterWriter_latest.h"
#include"EncoderBoard.h"
#include"LoopCycleController.h"
#include"PidController.h"
#include<Wire.h>

/*    外部リンケージ     */
int GetGyroOffset();
int GetGyro();

/*    constant     */


/*    instance     */
ControllerManager DS4(9);
EncoderBoard enc(5);
i2cMasterWriter motorBoard(1, 8);
i2cMasterWriter Solenoid(3, 5);
i2cMasterWriter Mecanum(2, 6, MECANUM);

static LoopCycleController loopCtrl(1000);     //ループ周期[us](ウデ)
static LoopCycleController loopMecanum(10000); //ループ周期[us](足)
static PidController pidCtrl(2.0, 0, 0.05, loopCtrl.Cycle_us(), 300.0);   //ウデPID
static PidController MecanumCorrection(4.5, 0, 0.5, loopMecanum.Cycle_us(), 300.0); //足PID
//比例係数 / 積分係数 / 微分係数 / ループ周期 / 積分量の最大値

//クリック処理用
struct buttonData {
  bool oldData = 0;
  bool newData = 0;
};

/*    variable    */
int referenceValue = 0;
int offset;

buttonData Touch, Triangle;

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
    Mecanum.Reset(MECANUM);
  }
}

/*    ジャイロセンサ     */
void Gyro() {

  GetGyroOffset();

  //DS4タッチパッド押下で初期位置取得
  Touch.newData = DS4.GetTouching();

  if (Touch.oldData == 0 && Touch.newData == 1)
    offset = GetGyroOffset();

  Touch.oldData = Touch.newData;

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
      referenceValue++;
    else if (DS4.GetR1())
      referenceValue--;
    }
    //45°傾ける
    else {
    if (DS4.GetL1())
      referenceValue = 300;
    else if (DS4.GetR1())
      referenceValue = -300;
    }
  
/*
  if (DS4.GetL1())
    motorBoard(3, 127);
  else if (DS4.GetR1())
    motorBoard(3, -127);
  else
    motorBoard(3, 0);
*/
  //直立
  Triangle.newData = DS4.GetTriangle();
  if (Triangle.oldData == 0 && Triangle.newData == 1)
    referenceValue = 0;
  Triangle.oldData = Triangle.newData;

  //PID
  pidCtrl.Update(enc.GetCount(), referenceValue);
  motorBoard.SetData(3, pidCtrl.GetPower());


  //motorBoard.SetData(3, 0);
}

/*    メカナム     */
void Tire() {
  //PID
  MecanumCorrection.Update(GetGyro(), 0);

  Mecanum.SetSingleData(0, DS4.GetLeftHatX());
  Mecanum.SetSingleData(1, DS4.GetLeftHatY());

  //↑ボタンでジャイロ補正のON/OFF
  if (DS4.GetUpState())
    Mecanum.SetSingleData(2, map(MecanumCorrection.GetPower(), -255, 255, 0, 255));
  else
    Mecanum.SetSingleData(2, DS4.GetRightHatX());

  Mecanum.SetSingleData(3, DS4.GetL3());
  Mecanum.SetSingleData(4, DS4.GetR3());
  Mecanum.SetSingleData(5, DS4.GetStartCount());
}

/*    ソレノイド    */
void solenoid() {
  Solenoid.SetSingleData(0, DS4.GetCircleState());
  Solenoid.SetSingleData(1, DS4.GetCrossState());
  Solenoid.SetSingleData(2, DS4.GetTriState());
  Solenoid.SetSingleData(3, DS4.GetSquareState());
  Solenoid.SetSingleData(4, DS4.GetShare_TriState());
}
