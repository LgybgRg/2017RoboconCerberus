#pragma once

class Mecanum {
  private:
    int i2cAddress;
    byte data[5] = {127, 127, 127, 0, 0};

  public:
    Mecanum(int i2c_address) {
      i2cAddress = i2c_address;
    }

    void WriteData(byte array_num, int stick) {
      data[array_num] = stick;
    }

    void Update() {
      Wire.beginTransmission(i2cAddress);
      Wire.write(data, 5);
      Wire.endTransmission();
    }

    void Reset() {
      data[0] = 127;
      data[1] = 127;
      data[2] = 127;
      data[3] = 0;
      data[4] = 0;
    }
};

