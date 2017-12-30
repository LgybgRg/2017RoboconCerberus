#ifndef DEF_ControllerManager_H
#define DEF_ControllerManager_H

#include <Wire.h>

constexpr int16_t CutAnalogValue = 20;  // アナログスティックの中間不感値
constexpr int16_t CutValue = CutAnalogValue * 2;

//[revice]START -> PSに変更

class ControllerManager {

  private:

    bool Select, L3, R3,  START, PS, Up, Right, Down, Left;
    bool L2, R2, L1, R1, Triangle, Circle, Cross, Square;

    int16_t LeftHatX, LeftHatY;
    int16_t RightHatX, RightHatY;
    int16_t AnalogL2, AnalogR2;

    int16_t theta;
    uint16_t r;
    int16_t LeftxAxis, LeftyAxis, RightxAxis;

    const uint8_t I2C_Address;

  public:
    ControllerManager(uint8_t Address) : I2C_Address(Address) {}

    void Update() {
      Wire.requestFrom(I2C_Address, 9);
      uint8_t buf[] = { 0, 0, 127, 127, 127, 127, 0, 0, 1 };
      while (Wire.available()) {
        buf[0] = Wire.read();
        buf[1] = Wire.read();
        buf[2] = Wire.read();
        buf[3] = Wire.read();
        buf[4] = Wire.read();
        buf[5] = Wire.read();
        buf[6] = Wire.read();
        buf[7] = Wire.read();
        buf[8] = Wire.read();
      }
      Select = bitRead(buf[0], 0);
      L3 = bitRead(buf[0], 1);
      R3 = bitRead(buf[0], 2);
      START = bitRead(buf[0], 3);
      Up = bitRead(buf[0], 4);
      Right = bitRead(buf[0], 5);
      Down = bitRead(buf[0], 6);
      Left = bitRead(buf[0], 7);

      L2 = bitRead(buf[1], 0);
      R2 = bitRead(buf[1], 1);
      L1 = bitRead(buf[1], 2);
      R1 = bitRead(buf[1], 3);
      Triangle = bitRead(buf[1], 4);
      Circle = bitRead(buf[1], 5);
      Cross = bitRead(buf[1], 6);
      Square = bitRead(buf[1], 7);

      PS = buf[8];

      /*
        RightHatX = buf[2] - 127;
        RightHatY = -(buf[3] - 127);
        LeftHatX = buf[4] - 127;
        LeftHatY = -(buf[5] - 127);
      */

      RightHatX = buf[2];
      RightHatY = buf[3];
      LeftHatX = buf[4];
      LeftHatY = buf[5];
      AnalogL2 = buf[6];
      AnalogR2 = buf[7];

      r = sqrt((LeftHatX * LeftHatX) + (LeftHatY * LeftHatY)) * 2;
      r = constrain(r, 0, 255);

      float Rad = atan2(LeftHatY, LeftHatX);
      theta = round(degrees(Rad));
      if (theta < 0) theta += 360;
      if (theta > 359) theta -= 360;

      if (r < CutValue) {
        r = 0;
        LeftxAxis = 0;
        LeftyAxis = 0;
      }
      else {
        r = map(r, CutValue, 255, 0, 255);
        LeftxAxis = r * cos(radians((float)theta));
        LeftyAxis = r * sin(radians((float)theta));
      }

      if (-CutAnalogValue < RightHatX && RightHatX < CutAnalogValue) {
        RightxAxis = 0;
      }
      else {
        if (RightHatX < 0) RightxAxis = map(RightHatX, -127, -CutAnalogValue, -127, 0);
        else RightxAxis = map(RightHatX, CutAnalogValue, 128, 0, 128);
      }
    }

    bool GetPS() {
      return PS;
    }
    bool GetStart() {
      return START;
    }
    bool GetSelect() {
      return Select;
    }
    bool GetL3() {
      return L3;
    }
    bool GetR3() {
      return R3;
    }
    bool GetUp() {
      return Up;
    }
    bool GetRight() {
      return Right;
    }
    bool GetDown() {
      return Down;
    }
    bool GetLeft() {
      return Left;
    }
    bool GetL2() {
      return L2;
    }
    bool GetR2() {
      return R2;
    }
    bool GetL1() {
      return L1;
    }
    bool GetR1() {
      return R1;
    }
    bool GetTriangle() {
      return Triangle;
    }
    bool GetCircle() {
      return Circle;
    }
    bool GetCross() {
      return Cross;
    }
    bool GetSquare() {
      return Square;
    }

    int16_t GetLeftHatX() {
      return LeftHatX;
    }
    int16_t GetLeftHatY() {
      return LeftHatY;
    }
    int16_t GetRightHatX() {
      return RightHatX;
    }
    int16_t GetRightHatY() {
      return RightHatY;
    }
    int16_t GetAnalogL2() {
      return AnalogL2;
    }
    int16_t GetAnalogR2() {
      return AnalogR2;
    }

    int16_t Gettheta() {
      return theta;
    }
    uint16_t Getr() {
      return r;
    }
    int16_t GetLeftxAxis() {
      return LeftxAxis;
    }
    int16_t GetLeftyAxis() {
      return LeftyAxis;
    }
    int16_t GetRightxAxis() {
      return RightxAxis;
    }

};

#endif

