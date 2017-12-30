//[fix] 2017/09/13 ウデPID微調整
//[mod] 2017/09/15 4点バースト仕様変更（十字ボタン -> 単発 / ○ボタン -> バースト)

/*    include     */
#include"ControllerManager.h"
#include"i2cMasterWriter.h"
#include"EncoderBoard.h"
#include"LoopCycleController.h"
#include"PidController.h"
#include"sub.h"
#include<Wire.h>

/*    constance      */
const int CONTROLLER_ADDRESS = 9;
const int ENCODER_ADDRESS_X = 5;
const int ENCODER_ADDRESS_Y = 8;
const int VALVE_ADDRESS = 3;
const int OMNI_ADDRESS = 2;

/*    instance     */
ControllerManager DS4(CONTROLLER_ADDRESS);  //コントローラ用オブジェクト
EncoderBoard enc_x(ENCODER_ADDRESS_X);      //エンコーダオブジェクト
EncoderBoard enc_y(ENCODER_ADDRESS_Y);      //エンコーダオブジェクト

i2cMasterWriter Solenoid(VALVE_ADDRESS, 5), Omni(OMNI_ADDRESS, 5, 1);  //ウデ，電磁弁，足回りのデータ送信用オブジェクト

static LoopCycleController loopCtrl(1000);     //ループ周期[us](ウデ)
static LoopCycleController loopOmni(10000); //ループ周期[us](足)

static PidController OmniCorrection(4.5, 0, 0.5, loopOmni.Cycle_us(), 300.0); //足PIDオブジェクト
static PidController ModExtPower_X(0.01, 0.1, 0.001,  loopCtrl.Cycle_us(), 300.0);
static PidController ModExtPower_Y(0.01, 0.1, 0.001, loopCtrl.Cycle_us(), 300.0);

static PidController AutoRun_X(0.1, 0, 0,  loopCtrl.Cycle_us(), 300.0);
static PidController AutoRun_Y(0.1, 0, 0, loopCtrl.Cycle_us(), 300.0);
//比例係数 / 積分係数 / 微分係数 / ループ周期 / 積分量の最大値

//クリック処理用
struct button_data {
  bool oldData = 0;
  bool newData = 0;
};

button_data Counter, Counter_L;

/*    variable    */
int ReferenceValue = 0;
int offset = 0;
bool Serialflag;

//常時ジャイロ補正用変数
static int counter, flag_counter;
int flag;

static long old_enc_x, old_enc_y;
bool counter_L, flag_L;

static long reference_x, reference_y;

void setup() {

  delay(2000);
  Wire.begin();
  Serial.begin(115200);
  Wire.setClock(400000UL); // Set I2C frequency to 400kHz

  enc_x.Init();            //基板の初期化(Wire.begin()の後に呼び出す)
  enc_x.SetPullup(EncoderBoard::PULLUP_ENABLE);
  enc_x.ResetCount();      //カウントのリセット

  enc_y.Init();            //基板の初期化(Wire.begin()の後に呼び出す)
  enc_y.SetPullup(EncoderBoard::PULLUP_ENABLE);
  enc_y.ResetCount();      //カウントのリセット

  OmniCorrection.ClearPower();
  ModExtPower_X.ClearPower();
  ModExtPower_Y.ClearPower();

  AutoRun_X.ClearPower();
  AutoRun_Y.ClearPower();
}

void loop() {
  AllUpdate();

  Tire();
  solenoid();

  //コントローラー切断時
  if (DS4.GetPS()) {
    Solenoid.Reset();
    Omni.Reset();
  }
}

void AllUpdate() {
  GetGyroOffset();
  enc_x.Update();
  enc_y.Update();
  DS4.Update();
  Omni.Update();
  loopCtrl.Update();
  Solenoid.Update();
}

/*    メカナム     */
void Tire() {

  Omni_gyroCorrection();
  if (DS4.GetLeft() == 0 && DS4.GetUp() == 0) {
    Omni.SetSingleData(0, DS4.GetLeftHatX());
    Omni.SetSingleData(1, DS4.GetLeftHatY());
  }
  else {
    if (DS4.GetLeft()) {
      reference_x = -8324;
      reference_y = 0;
    }

    if (DS4.GetUp()) {
      reference_x = -30965;
      reference_y = 56821;
    }

    AutoRun_X.Update(enc_x.GetCount(), reference_x);
    AutoRun_Y.Update(enc_y.GetCount(), reference_y);

    Omni.SetSingleData(0, map(AutoRun_X.GetPower(), -255, 255, 255, 0));
    Omni.SetSingleData(1, map(AutoRun_Y.GetPower(), -255, 255, 0, 255));
  }

  Serial.print(enc_x.GetCount()); Serial.print("\t");
  Serial.print(enc_y.GetCount()); Serial.print("\t");
   Serial.print(reference_x); Serial.print("\t");
  Serial.print(reference_y); Serial.print("\t");
  Serial.print(AutoRun_X.GetPower());   Serial.print("\t");
  Serial.println(AutoRun_Y.GetPower());


  /*Serial.print(enc_x.GetCount()); Serial.print("\t");
    Serial.println(enc_y.GetCount());
  */

  Omni.SetSingleData(3, !DS4.GetCrossToggle());
  Omni.SetSingleData(4, DS4.GetStartToggle());
}

void Omni_gyroCorrection() {

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
      OmniCorrection.Update(GetGyro(), 0);
      Omni.SetSingleData(2, map(OmniCorrection.GetPower(), -255, 255, 0, 255));
    }
    else {
      Omni.SetSingleData(2, DS4.GetRightHatX());
    }
  }
  else {
    Omni.SetSingleData(2, DS4.GetRightHatX());
  }
}

/*    ソレノイド    */
void solenoid() {
  static unsigned long int circle_counter, portSelecter = 1;
  static int start_signal;
  static bool flag;

  //単発
  Solenoid.SetSingleData(0, DS4.GetL3Toggle());
  /* if (flag == false) {
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
       Solenoid.SetSingleData(4 , 0);
    }
  */

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

  //Solenoid.Show();
}

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


/*
  if ((DS4.GetLeftHatX() > 120 && DS4.GetLeftHatX() < 130 ) || ( DS4.GetLeftHatY() > 120 && DS4.GetLeftHatY() < 130)) {
    counter_L = 1;
  }
  else
    counter_L = 0;

  Counter_L.newData = counter_L;

  if (Counter_L.oldData == 0 && Counter_L.newData == 1)
    flag_L = 1;
  else
    flag_L = 0;

  Counter_L.oldData = Counter_L.newData;

  if (flag_L) {
    old_enc_x = enc_x.GetCount();
    old_enc_y = enc_y.GetCount();
    ModExtPower_X.ClearPower();
    ModExtPower_Y.ClearPower();

  }

  if (counter_L) {
    ModExtPower_X.Update(enc_x.GetCount(), old_enc_x);
    ModExtPower_Y.Update(enc_y.GetCount(), old_enc_y);

    Omni.SetSingleData(0, map(ModExtPower_X.GetPower(), -255, 255, 255, 0));
    Omni.SetSingleData(1, map(ModExtPower_Y.GetPower(), -255, 255, 0, 255));
  }
  else {
    Omni.SetSingleData(0, DS4.GetLeftHatX());
    Omni.SetSingleData(1, DS4.GetLeftHatY());
  }

  Serial.print(old_enc_x);
  Serial.print("\t");
  Serial.print(old_enc_y);
  Serial.print("\t");
  Serial.print(enc_x.GetCount());
  Serial.print("\t");
  Serial.print(enc_y.GetCount());
  Serial.print("\t");
  Serial.print(ModExtPower_X.GetPower());
  Serial.print("\t");
  Serial.println(ModExtPower_Y.GetPower());
*/

