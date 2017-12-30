//2017/09/13 ウデPID微調整
//2017/09/15 4点バースト仕様変更（十字ボタン -> 単発 / ○ボタン -> バースト)
//2017/09/20 足回りに送信するデータを変更
//2017/09/20 分割コンパイル
//2017/09/22 PidController.hの編集，MaxPowerを変更可能に
//2017/09/28 4点バースト前にピンを抜くモータを回すプログラムを実装
//2017/09/29 4点バーストのピンを抜くモータが一度きりしか動かないように修正

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
const int ARM_ADDRESS = 1;

/*    instance     */
ControllerManager DS4(CONTROLLER_ADDRESS);  //コントローラ用オブジェクト

EncoderBoard enc_x(ENCODER_ADDRESS_X);      //エンコーダオブジェクト
EncoderBoard enc_y(ENCODER_ADDRESS_Y);      //エンコーダオブジェクト

i2cMasterWriter Arm(ARM_ADDRESS, 8), Solenoid(VALVE_ADDRESS, 5), Omni(OMNI_ADDRESS, 6, 1); //ウデ，電磁弁，足回りのデータ送信用オブジェクト

LoopCycleController loopCtrl(1000);     //ループ周期[us](ウデ)
static LoopCycleController loopOmni(10000); //ループ周期[us](足)

PidController OmniCorrection(4.0, 0, 0.5, loopOmni.Cycle_us(), 300.0); //ジャイロ修正オブジェクト
PidController AutoRun(0.015, 0, 0.001, loopCtrl.Cycle_us(), 300.0);    //自動走行オブジェクト

//static PidController ModExtPower(0.01, 0.1, 0.001, loopCtrl.Cycle_us(), 300.0);

//比例係数 / 積分係数 / 微分係数 / ループ周期 / 積分量の最大値

//クリック処理用

buttonData Counter, Counter_L;

/*    variable    */
int offset = 0;
bool Serialflag;  //ジャイロ通信スイッチ切替忘れ防止

void setup() {

  delay(2000);
  Wire.begin();
  Serial.begin(115200);
  Wire.setClock(400000UL); // Set I2C frequency to 400kHz

  EncoderInitialize();

  OmniCorrection.ClearPower();
  //ModExtPower.ClearPower();
  AutoRun.ClearPower();
}

void loop() {

  ALLUPDATE();

}
