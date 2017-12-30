/*    include     */
#include"ControllerManager.h"
#include"i2cMasterWriter.h"
#include"EncoderBoard.h"
#include"LoopCycleController.h"
#include"PidController.h"
#include"SerialDataLogger.h"
#include"Solenoid.h"
#include"Mecanum.h"
#include<Wire.h>

/*    constant     */

/*    instance     */
ControllerManager DS3(9);
EncoderBoard enc(5);
i2cMasterWriter motorBoard(1);
Solenoid Solenoid(3);
Mecanum Mecanum(2);
SerialDataLogger pidData(3);
static LoopCycleController loopCtrl(1000);     //ループ周期[us]
static PidController pidCtrl(0.8, 0.04, 0.055, loopCtrl.Cycle_us(), 300.0);
//比例係数 / 積分係数 / 微分係数 / ループ周期 / 積分量の最大値


/*    variable    */
int referenceValue = 0;
static int upcount, downcount, rightcount, leftcount, squarecount;
static int CircleCount, CrossCount, TriCount, SquareCount;
static byte turn;

void setup() {

  Wire.begin();
  Serial.begin(115200);
  //Wire.setClock(400000UL); // Set I2C frequency to 400kHz

  enc.Init();            //基板の初期化(Wire.begin()の後に呼び出す)
  enc.SetPullup(EncoderBoard::PULLUP_ENABLE);
  enc.ResetCount();      //カウントのリセット

  pidCtrl.ClearPower();   //操作量の値をすべてクリア
}

void loop() {

  AllUpdate();

  motor_board();
  Tire();

  
  //コントローラー切断時
  if (DS3.GetPS()) {
    Solenoid.Reset();
    Mecanum.Reset();
  }
}
/*
   void data_logger() {
  pidData.SetX((float)micros() / 1000000.0f);
  pidData.SetSeriesY(0, referenceValue);
  pidData.SetSeriesY(1, (double)enc.GetCount());
  pidData.SetSeriesY(2, pidCtrl.GetPower());
  pidData.Send();
  }
*/

void AllUpdate() {
  enc.Update();
  DS3.Update();
  motorBoard.Update();
  Mecanum.Update();
  loopCtrl.Update();
  //Solenoid.Update();
}

//motorBoardへ送信するデータ処理
void motor_board() {
  //ウデ
  if (DS3.GetR2())
    motorBoard.WriteData(1, 255);
  else if (DS3.GetL2())
    motorBoard.WriteData(1, -255);
  else
    motorBoard.WriteData(1, 0);

  //傾け
  if (DS3.GetSelect()) {
    if (DS3.GetL1())
      referenceValue++;
    else if (DS3.GetR1())
      referenceValue--;
  }
  else {
    if (DS3.GetL1())
      referenceValue = 300;
    else if (DS3.GetR1())
      referenceValue = -300;
  }
  if (DS3.GetTriangle())
    referenceValue = 0;

  pidCtrl.Update(enc.GetCount(), referenceValue);
  motorBoard.WriteData(3, pidCtrl.GetPower());
}

//足
void Tire() {
  Mecanum.WriteData(0, DS3.GetLeftHatX());
  Mecanum.WriteData(1, DS3.GetLeftHatY());
  Mecanum.WriteData(2, DS3.GetRightHatX());
  Mecanum.WriteData(3, DS3.GetL3());
  Mecanum.WriteData(4, DS3.GetR3());
}
