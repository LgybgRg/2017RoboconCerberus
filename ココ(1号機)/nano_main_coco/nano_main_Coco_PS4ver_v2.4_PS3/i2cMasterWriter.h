#pragma once
#include<arduino.h>
#include<Wire.h>

class i2cMasterWriter {
  private:
    const int i2c_address;
    byte* data;
    int Size;
    int _option;

  public:
    i2cMasterWriter(int Address, int array_size, bool option = 0) : i2c_address(Address), _option(option), Size(array_size) {

      //足用に配列を初期化
      if (option == 1) {
        data = new byte[array_size];
        data[0] = 0;
        data[3] = 127;
      }
      else {
        data = new byte[array_size];
        for (int i = 0; i < array_size; i++)
          data[i] = 0;
      }
    }

    void Show() {
      for (int i = 0; i < Size; i++) {
        Serial.print(data[i]);
        Serial.print("\t");
      }
      Serial.println("");
    }

    //デストラクタ
    virtual ~i2cMasterWriter() {
      delete[] data;
    }

    //SingleData
    void SetSingleData(int array_num, byte Data) {
      data[array_num] = Data;
    }

    //MotorBoard
    void SetData(byte portNum, int16_t PWM) {
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
    //bitWrite
    void SetBitData(int array_num, byte bit_num, bool bit) {
      bitWrite(data[array_num], bit_num, bit);
    }

    void Reset() {
      //足用
      if (_option == 1) {
        data[0] = 0;
        data[3] = 127;
      }
      else {
        for (int i = 0; i < Size; i++) {
          data[i] = 0;
        }
      }
    }

    // Slave側へ送信
    void Update() {
      Wire.beginTransmission(i2c_address);
      Wire.write(data, Size);
      Wire.endTransmission();
    }
};

extern i2cMasterWriter Solenoid,Omni,Arm;
