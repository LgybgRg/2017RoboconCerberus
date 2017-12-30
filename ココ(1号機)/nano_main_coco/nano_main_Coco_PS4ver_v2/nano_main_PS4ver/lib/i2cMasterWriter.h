#pragma once

class i2cMasterWriter {

  private:
    int i2c_address;
    byte data[8] = {0, 0, 0, 0, 0, 0, 0, 0};

  public:
    i2cMasterWriter(int Address) {
      i2c_address = Address;
    }

    /*
         MotorBoardへ送るデータの設定

         @argument  portNum   ポート番号
         @argument  Mode      0=フリー,1=正転，2=逆転，3=ブレーキ
         @argument  PWM       PWM値
    */
    void WriteData(byte portNum, int PWM) {
      byte Mode;
      if (PWM == 0)
        Mode = 3;

      else if (PWM > 0)
        Mode = 1;

      else
        Mode = 2;

      PWM = abs(PWM);
      switch (portNum) {
        case 1:
          data[0] = Mode;
          data[1] = PWM;  break;
        case 2:
          data[2] = Mode;
          data[3] = PWM;  break;
        case 3:
          data[4] = Mode;
          data[5] = PWM;  break;
        case 4:
          data[6] = Mode;
          data[7] = PWM;  break;
      }
    }
    
    void Reset() {
      for (int i = 0; i < 8; i++) {
        data[i] = 0;
      }
    }

    // Slave側へ送信
    void Update() {
      Wire.beginTransmission(i2c_address);
      Wire.write(data, 8);
      Wire.endTransmission();
    }
};

