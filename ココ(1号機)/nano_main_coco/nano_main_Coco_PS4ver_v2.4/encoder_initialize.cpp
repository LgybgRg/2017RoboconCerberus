#include<arduino.h>
#include"EncoderBoard.h"

void EncoderInitialize() {
  enc_x.Init();            //基板の初期化(Wire.begin()の後に呼び出す)
  enc_x.SetPullup(EncoderBoard::PULLUP_ENABLE);
  enc_x.ResetCount();      //カウントのリセット

  enc_y.Init();            //基板の初期化(Wire.begin()の後に呼び出す)
  enc_y.SetPullup(EncoderBoard::PULLUP_ENABLE);
  enc_y.ResetCount();      //カウントのリセット

}

