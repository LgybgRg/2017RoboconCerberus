//[fix] 2017/09/13 ウデPID微調整
//[add] 2017/09/15 4点バースト仕様変更（十字ボタン -> 単発 / ○ボタン -> バースト

/*    include     */
#include"ControllerManager.h"
#include"i2cMasterWriter.h"
#include"EncoderBoard.h"
#include"LoopCycleController.h"
#include"PidController.h"
#include"sub.h"
#include<Wire.h>

/*    constance      */

/*    instance     */
ControllerManager DS4(9);  //コントローラ用オブジェクト
EncoderBoard enc(5);       //エンコーダオブジェクト
i2cMasterWriter motorBoard(1, 8), Solenoid(3, 5), Mecanum(2, 6, 1);  //ウデ，電磁弁，足回りのデータ送信用オブジェクト

static LoopCycleController loopCtrl(1000);     //ループ周期[us](ウデ)
static LoopCycleController loopMecanum(10000); //ループ周期[us](足)

static PidController pidCtrl(2.0, 0, 0.05, loopCtrl.Cycle_us(), 300.0);   //ウデPIDオブジェクト
static PidController MecanumCorrection(6.5, 0, 0.8, loopMecanum.Cycle_us(), 300.0); //足PIDオブジェクト
//比例係数 / 積分係数 / 微分係数 / ループ周期 / 積分量の最大値

//クリック処理用
struct button_data {
  bool oldData = 0;
  bool newData = 0;
};

button_data Counter;

/*    variable    */
int ReferenceValue = 0;
int offset = 0;
bool Serialflag;


//常時ジャイロ補正用変数
static int counter, flag_counter;
int flag;

void setup() {

  delay(2000);
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

  //コントローラー切断時
  if (DS4.GetPS()) {
    Solenoid.Reset();
    motorBoard.Reset();
    Mecanum.Reset();
  }
}

void AllUpdate() {
  GetGyroOffset();
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
    motorBoard.SetData(1, -255);
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

  else {
    //45°傾ける
    if (DS4.GetL1())
      ReferenceValue = 916;
    else if (DS4.GetR1())
      ReferenceValue = -972;
    //直立
  }
  if (DS4.GetTriangle())
    ReferenceValue = 0;

  //PID
  pidCtrl.Update(enc.GetCount(), ReferenceValue);
  motorBoard.SetData(3, pidCtrl.GetPower());
}

/*    メカナム     */
void Tire() {
  /*
    if (DS4.GetUpClick())
    MecanumCorrection.SetParamPro(MecanumCorrection.GetParamPro() + 0.1);
    else if (DS4.GetDownClick())
    MecanumCorrection.SetParamPro(MecanumCorrection.GetParamPro() - 0.1);

    if (MecanumCorrection.GetParamPro() < 0)
    MecanumCorrection.SetParamPro(0);

    if (DS4.GetRightClick())
    MecanumCorrection.SetParamDif(MecanumCorrection.GetParamDif() + 0.1);
    else if (DS4.GetLeftClick())
    MecanumCorrection.SetParamDif(MecanumCorrection.GetParamDif() - 0.1);

    if (MecanumCorrection.GetParamDif() < 0)
    MecanumCorrection.SetParamDif(0);

    Serial.print(MecanumCorrection.GetParamPro());
    Serial.print("\t");
    Serial.println(MecanumCorrection.GetParamDif());
  */


  if (DS4.GetTouchToggle() == 0 && Serialflag == 1) {
    if (DS4.GetRightHatX() > 120 && DS4.GetRightHatX() < 130) {
      counter = 1;
    }
    else
      counter = 0;

    Counter.newData = counter;

    if (Counter.oldData == 0 && Counter.newData == 1)
      flag = 1;

    Counter.oldData = Counter.newData;

    if (counter == 1 && flag == 1)
      flag_counter++;

    if (flag_counter > 400) {
      offset = GetGyroOffset();
      flag = 0;
      flag_counter = 0;
    }

    if (counter == 1 && flag == 0) {
      MecanumCorrection.Update(GetGyro(), 0);
      Mecanum.SetSingleData(2, map(MecanumCorrection.GetPower(), -255, 255, 0, 255));
    }
    else {
      Mecanum.SetSingleData(2, DS4.GetRightHatX());
    }
  }
  else
    Mecanum.SetSingleData(2, DS4.GetRightHatX());

  Mecanum.SetSingleData(0, DS4.GetLeftHatX());
  Mecanum.SetSingleData(1, DS4.GetLeftHatY());

  Mecanum.SetSingleData(3, !DS4.GetCrossToggle());
  Mecanum.SetSingleData(4, 0);
  Mecanum.SetSingleData(5, DS4.GetStartToggle());

}

/*    ソレノイド    */
void solenoid() {
  static unsigned long int circle_counter, portSelecter = 1;
  static int start_signal;
  static bool flag;

  //単発
  Solenoid.SetSingleData(0, DS4.GetL3Toggle());
  if(flag==false){
    if (DS4.GetUp())
    Solenoid.SetSingleData(1, 1);
    else
    Solenoid.SetSingleData(1, 0);

    if (DS4.GetRight())
    Solenoid.SetSingleData(2, 1);
    else
    Solenoid.SetSingleData(2, 0);

    if (DS4.GetDown())
    Solenoid.SetSingleData(3, 1);
    else
    Solenoid.SetSingleData(3, 0);

    if (DS4.GetLeft())
    Solenoid.SetSingleData(4, 1);
    else
    Solenoid.SetSingleData(4 ,0);
  }

    /*
    Solenoid.SetSingleData(1, DS4.GetUp());
    Solenoid.SetSingleData(2, DS4.GetRight());
    Solenoid.SetSingleData(3, DS4.GetDown());
    Solenoid.SetSingleData(4, DS4.GetLeft());
  */

  //単発(オート)
  /*
    if (DS4.GetCircleClick()) {
      circle_counter = millis();
      Solenoid.SetSingleData(1, 1);
      portSelecter = 1;
      start_signal = 1;
    }
    if (millis() - circle_counter >=300 && portSelecter <= 4 && start_signal == 1) {
      Solenoid.SetSingleData(portSelecter, 0);
      portSelecter++;
      Solenoid.SetSingleData(portSelecter, 1);
      circle_counter = millis();
    }
  */

  //2点同時(オート)
  if (DS4.GetCircleClick()) {
    circle_counter = millis();
    Solenoid.SetSingleData(1, 1);
    Solenoid.SetSingleData(3, 1);
    start_signal = 1;
    flag = true;
  }
  if (millis() - circle_counter >= 300 && start_signal == 1 && flag == true) {
    Solenoid.SetSingleData(1, 0);
    Solenoid.SetSingleData(3, 0);

    Solenoid.SetSingleData(2, 1);
    Solenoid.SetSingleData(4, 1);
  }

  if (millis() - circle_counter >= 600 && start_signal == 1 && flag == true) {
    Solenoid.SetSingleData(2, 0);
    Solenoid.SetSingleData(4, 0);
    flag = false;
  }

  Solenoid.Show();
}

