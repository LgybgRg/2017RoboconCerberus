/*    include     */
#include"ControllerManager.h"
#include"i2cMasterWriter.h"
#include"EncoderBoard.h"
#include"LoopCycleController.h"
#include"PidController.h"
#include"Solenoid.h"
#include"Mecanum.h"
//#include"i2cMasterWriter_latest.h"
#include<Wire.h>

/*    外部リンケージ     */
int GetGyro();
int GetGyroOfset();

/*    constant     */

/*    instance     */
ControllerManager DS4(9);
//EncoderBoard enc(20);
i2cMasterWriter motorBoard(1);
Solenoid Solenoid(3);
Mecanum Mecanum(2);
static LoopCycleController loopMecanum(10000);
static PidController MecanumCorrection(5.5, 0, 0.5, loopMecanum.Cycle_us(), 300.0);
//比例係数 / 積分係数 / 微分係数 / ループ周期 / 積分量の最大値

struct buttonData {
  bool oldData = 0;
  bool newData = 0;
};

buttonData Touch, Triangle;

/*    variable    */
int ofset;
static byte power;

void setup() {

  Wire.begin();
  Serial.begin(115200);
  //Wire.setClock(400000UL); // Set I2C frequency to 400kHz
  /*
    enc.Init();            //基板の初期化(Wire.begin()の後に呼び出す)
    enc.SetPullup(EncoderBoard::PULLUP_ENABLE);
    enc.ResetCount();      //カウントのリセット
  */
  MecanumCorrection.ClearPower();//操作量の値をすべてクリア

}

void loop() {

  AllUpdate();

  motor_board();
  Tire();
  solenoid();
  Gyro();
  edf();


  //コントローラー切断時
  if (DS4.GetPS()) {
    Solenoid.Reset();
    Mecanum.Reset();
    power = 0;
  }
}

void AllUpdate() {
  //enc.Update();
  DS4.Update();
  motorBoard.Update();
  Mecanum.Update();
  Solenoid.Update();
  edfUpdate();
}

/*    エレクトリック抱くテッドファン      */
void edf() {

  if (DS4.GetCircleState() == 1) {
    power = 180;
  }
  else
    power = 0;
}

void edfUpdate() {
  Wire.beginTransmission(4);
  Wire.write(power);
  Wire.endTransmission();
}

void Gyro() {
  GetGyroOfset();

  Touch.newData = DS4.GetTouching();

  if (Touch.oldData == 0 && Touch.newData == 1)
    ofset = GetGyroOfset();

  Touch.oldData = Touch.newData;

  /*
     Serial.print(ofset);
     Serial.print("\t");
     Serial.println(GetGyro());
  */
}

//motorBoardへ送信するデータ処理
void motor_board() {

  //第一関節
  if (DS4.GetR2())
    motorBoard.WriteData(1, DS4.GetAnalogR2());
  else if (DS4.GetL2())
    motorBoard.WriteData(1, -DS4.GetAnalogL2());
  else
    motorBoard.WriteData(1, 0);

  //第二関節
  if (DS4.GetL1())
    motorBoard.WriteData(3, -127);
  else if (DS4.GetR1())
    motorBoard.WriteData(3, 127);
  else
    motorBoard.WriteData(3, 0);
  /*
     //第一関節
    if (DS4.GetR2())
    motorBoard.WriteData(1, DS4.GetAnalogR2());
    else if (DS4.GetL2())
    motorBoard.WriteData(1, -DS4.GetAnalogL2());
    else
    motorBoard.WriteData(1, 0);

    //第二関節
    if (DS4.GetL1())
    motorBoard.WriteData(3, -127);
    else if (DS4.GetR1())
    motorBoard.WriteData(3, 127);
    else
    motorBoard.WriteData(3, 0);
  */
}

//足
void Tire() {
  MecanumCorrection.Update(GetGyro(), 0);
  Mecanum.WriteData(0, DS4.GetLeftHatX());
  Mecanum.WriteData(1, DS4.GetLeftHatY());

  if (DS4.GetUpState())
    Mecanum.WriteData(2, map(MecanumCorrection.GetPower(), -255, 255, 0, 255));
  else
    Mecanum.WriteData(2, DS4.GetRightHatX());

  Mecanum.WriteData(3, DS4.GetL3());
  Mecanum.WriteData(4, DS4.GetR3());
  Mecanum.WriteData(5, DS4.GetStartCount());
}

//solenoid
void solenoid() {
  Solenoid.WriteData(0, DS4.GetCircleState());
  Solenoid.WriteData(1, DS4.GetCrossState());
  Solenoid.WriteData(2, DS4.GetTriState());
  Solenoid.WriteData(3, DS4.GetSquareState());
  Solenoid.WriteData(4, DS4.GetShare_TriState());
}
