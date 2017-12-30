#pragma once

#include"MovingAverage.h"

const int M1A = 2;
const int M1B = 4;
const int M1PWM = 3;

const int M2A = 6;
const int M2B = 5;
const int M2PWM = 9;

const int M3A = 12;
const int M3B = A0;
const int M3PWM = 11;

const int M4A = 8;
const int M4B = 7;
const int M4PWM = 10;

const int BorderValue = 20;         //ジョイスティックの不感範囲
const double TurnParameter = 0.30;

class Omni {

  private:

    const uint16_t  MaxPower;
    double r;
    double m_angle;
    double moving_axis_x, moving_axis_y, turn_axis_x;

    MovingAverage average1;
    MovingAverage average2;
    MovingAverage average3;
    MovingAverage average4;

  public:

    /*
        [Constructor]
        @param 　max_power　  PWM出力の最大値
        @param 　average   　 移動平均するデータ数
    */

    Omni(const int max_power, const int averagesize):
      average1(averagesize), average2(averagesize), average3(averagesize), average4(averagesize), MaxPower(min(max_power, 255)) {

      pinMode(M1A, OUTPUT);
      pinMode(M1B, OUTPUT);
      pinMode(M2A, OUTPUT);
      pinMode(M2B, OUTPUT);
      pinMode(M3A, OUTPUT);
      pinMode(M3B, OUTPUT);
      pinMode(M4A, OUTPUT);
      pinMode(M4B, OUTPUT);

    }

    //PWM周波数の変更
    void SetPWMFrequency() {
      TCCR1B = (TCCR1B & 0b11111000) | 0x02;
      TCCR2B = (TCCR2B & 0b11111000) | 0x02;
    }

    //スティックの傾きを取得
    double GetStickSlope(double xComponent, double yComponent) {

      double Slope = constrain(sqrt(pow(yComponent, 2) + pow(xComponent, 2)), 0, MaxPower);

      return Slope;
    }

    //スティックの角度を取得
    double GetAngle(double xComponent, double yComponent) {

      double angle = atan2(yComponent, xComponent);

      if (angle < 0)
        angle += 2 * PI;

      return angle;
    }

    /*
       制御に用いるジョイスティックの軸を設定

       @param  MovingAxis_X  　移動軸X
       @param  MovingAxis_Y  　移動軸Y
       @param  TurnAxis_X    　旋回軸X
       @param  lowSpeed_flag 　低速モード On/Off
    */
    void SetAxis(int MovingAxis_X, int MovingAxis_Y, int TurnAxis_X, bool lowSpeed_flag) {
      moving_axis_x = MovingAxis_X;
      moving_axis_y = MovingAxis_Y;
      turn_axis_x = TurnAxis_X;

      if (lowSpeed_flag == true) {
        moving_axis_x = -map(moving_axis_x, 0, 255, -MaxPower / 4, MaxPower / 4);
        moving_axis_y = -map(moving_axis_y, 255, 0, -MaxPower / 4, MaxPower / 4);
      }
      else {
        moving_axis_x = -map(moving_axis_x, 0, 255, -MaxPower, MaxPower);
        moving_axis_y = -map(moving_axis_y, 255, 0, -MaxPower, MaxPower);
      }
      turn_axis_x = -map(turn_axis_x, 0, 255, -MaxPower, MaxPower);
      if (abs(turn_axis_x) < BorderValue)
        turn_axis_x = 0;
      else
        turn_axis_x =  map(turn_axis_x, -MaxPower, MaxPower, MaxPower * TurnParameter, -MaxPower * TurnParameter);

    }

    /*
        三角関数によるモータの出力計算
         @param  モータ出力値
    */
    void Set4WheelPower(int16_t Power[]) {

      //int16_t Power[4];

      r = GetStickSlope(moving_axis_x, moving_axis_y);
      m_angle = GetAngle(moving_axis_x, moving_axis_y);

      if (r < BorderValue) {
        Power[0] = Power[1] = Power[2] = Power[3] = 0;
      }
      else {
        r = map(r, BorderValue, MaxPower, 0, MaxPower);

        //タイヤの設置角度
        Power[0] = r * sin(m_angle - (PI / 6));
        Power[1] = r * sin(m_angle - (5 * PI / 6));
        Power[2] = r * sin(m_angle - (7 * PI / 6));
        Power[3] = r * sin(m_angle - (11 * PI / 6));
      }

      /*   if (abs(turn_axis_x) > BorderValue) {
           if (turn_axis_x < 0)
             turn_axis_x = map(turn_axis_x, -MaxPower, -BorderValue, -MaxPower, 0);


           else
             turn_axis_x = map(turn_axis_x, BorderValue, MaxPower, 0, MaxPower);
      */
      // }

      //一番大きい値を取得
      double max_ = abs(Power[0]);

      for (int i = 1; i < 4; i++) {
        if (abs(Power[i]) > max_)
          max_ = abs(Power[i]);
      }

      double ratio;     //補正倍率
      if (max_ > BorderValue)
        ratio = r / max_;

      for (int i = 0; i < 4; i++)
        Power[i] *= ratio;

      Power[0] += turn_axis_x;
      Power[1] += turn_axis_x;
      Power[2] += turn_axis_x;
      Power[3] += turn_axis_x;

      max_ = abs(Power[0]);

      for (int i = 1; i < 4; i++) {
        if (abs(Power[i]) > max_)
          max_ = abs(Power[i]);
      }

      //範囲外であれば範囲内(MaxPowerまでの値)に収める
      if (max_ > MaxPower) {
        float amp = float(MaxPower) / max_;
        Power[0] = int16_t(Power[0] * amp);
        Power[1] = int16_t(Power[1] * amp);
        Power[2] = int16_t(Power[2] * amp);
        Power[3] = int16_t(Power[3] * amp);
      }

      //移動平均

      average1.Update(Power[0]);
      average2.Update(Power[1]);
      average3.Update(Power[2]);
      average4.Update(Power[3]);

      Power[0] = average1.GetValue();
      Power[1] = average2.GetValue();
      Power[2] = average3.GetValue();
      Power[3] = average4.GetValue();

    }


    void Drive(int16_t motorPower[]) {
      //    Motor1
      if (motorPower[0] == 0) {
        digitalWrite(M1A, HIGH);
        digitalWrite(M1B, HIGH);
        analogWrite(M1PWM, 0);
      }
      else if (motorPower[0] > 0) {
        digitalWrite(M1A, HIGH);
        digitalWrite(M1B, LOW);
        analogWrite(M1PWM, motorPower[0]);
      }
      else  {
        digitalWrite(M1A, LOW);
        digitalWrite(M1B, HIGH);
        analogWrite(M1PWM, abs(motorPower[0]));
      }

      //    Motor2
      if (motorPower[1] == 0) {
        digitalWrite(M2A, HIGH);
        digitalWrite(M2B, HIGH);
        analogWrite(M2PWM, 0);
      }
      else if (motorPower[1] > 0) {
        digitalWrite(M2A, HIGH);
        digitalWrite(M2B, LOW);
        analogWrite(M2PWM, motorPower[1]);
      }
      else  {
        digitalWrite(M2A, LOW);
        digitalWrite(M2B, HIGH);
        analogWrite(M2PWM, abs(motorPower[1]));
      }

      //    Motor3
      if (motorPower[2] == 0) {
        digitalWrite(M3A, HIGH);
        digitalWrite(M3B, HIGH);
        analogWrite(M3PWM, 0);
      }
      else if (motorPower[2] > 0) {
        digitalWrite(M3A, HIGH);
        digitalWrite(M3B, LOW);
        analogWrite(M3PWM, motorPower[2]);
      }
      else {
        digitalWrite(M3A, LOW);
        digitalWrite(M3B, HIGH);
        analogWrite(M3PWM, abs(motorPower[2]));

      }

      //    Motor4
      if (motorPower[3] == 0) {
        digitalWrite(M4A, HIGH);
        digitalWrite(M4B, HIGH);
        analogWrite(M4PWM, 0);
      }
      else if (motorPower[3] > 0) {
        digitalWrite(M4A, HIGH);
        digitalWrite(M4B, LOW);
        analogWrite(M4PWM, motorPower[3]);
      }
      else {
        digitalWrite(M4A, LOW);
        digitalWrite(M4B, HIGH);
        analogWrite(M4PWM, abs(motorPower[3]));
      }
    }
};
