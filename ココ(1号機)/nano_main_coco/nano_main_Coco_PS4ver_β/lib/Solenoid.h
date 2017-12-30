#pragma once

class Solenoid {
  private:
    int i2c_address;
    byte data[5] = {0, 0, 0, 0, 0};

  public:

    Solenoid(int i2cAddress) {
      i2c_address = i2cAddress;
    }

    void isOn(int array_num) {
      data[array_num] = 1;
    }

    void isOff(int array_num) {
      data[array_num] = 0;
    }

    void WriteData(int array_num, bool state) {
      data[array_num] = state;
    }

    void Update() {
      Wire.beginTransmission(i2c_address);
      Wire.write(data, 5);
      Wire.endTransmission();
    }

    void Reset() {
      for (int i = 0; i < 5; i++)
        data[i] = 0;
    }
};

