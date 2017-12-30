#pragma once

const int MaxPower = 100;


class Omni {
  public:
    Omni(int pin_A, int pin_B, int pin_Pwm, double angle, bool on_off);
    double caluculateAngle(double joystickHat_L_X, double joystickHat_L_Y, double joystickHat_R_X);
    int caluculatePower(double joystickHat_L_X, double joystickHat_L_Y);
    void setUpBorder(int borderValue) {
      border = borderValue;
    }
    int drive(double joystickHat_L_X, double joystickHat_L_Y, double joystickHat_R_X);
    void Run(int motorpower);
  private:
    int pinA;
    int pinB;
    int pinPwm;
    int border = 20;
    int motorPower;
    double joystickAngle;
    double motorAngle;
    double rotationCorrection;
    bool on_off;                 // 回転補正するやつのonOff決めるやつ
};

Omni::Omni(int pin_A, int pin_B, int pin_Pwm, double angle, bool on_or_off) {
  pinA   = pin_A;
  pinB   = pin_B;
  pinPwm = pin_Pwm;
  motorAngle = angle;
  on_off = on_or_off;
}

int Omni::caluculatePower(double joystickHat_L_X, double joystickHat_L_Y) {
  //motorPower = constrain(joystickHat_L_Y, joystickHat_L_X, 255+border);
  motorPower = sqrt( square(joystickHat_L_X) + square(joystickHat_L_Y) );
  motorPower = map(motorPower, 0, 357, 0, MaxPower);

  return motorPower;
}

double Omni::caluculateAngle(double joystickHat_L_X, double joystickHat_L_Y, double joystickHat_R_X) {

  const double rotationParameter = 0.01;
  static double rotationSum = 0;
  static bool judgement_now = false;
  static bool judgement_old = false;

  joystickAngle = atan2(joystickHat_L_Y, joystickHat_L_X);
  if (joystickAngle < 0)
    joystickAngle += 2 * PI;

  if (on_off == true) {
    if (border < abs(joystickHat_R_X)) {
      joystickAngle -= joystickHat_R_X * rotationParameter;
      rotationSum += joystickHat_R_X * rotationParameter;
      judgement_now = true;
    }
    else {
      judgement_now = false;
    }

    if (judgement_now == false && judgement_old == true) {
      joystickAngle += rotationSum;
      rotationSum = 0;
    }
    judgement_old = judgement_now;
  }

  return joystickAngle;
}


int Omni::drive(double joystickHat_L_X, double joystickHat_L_Y, double joystickHat_R_X) {

  caluculatePower(joystickHat_L_X, joystickHat_L_Y);
  caluculateAngle(joystickHat_L_X, joystickHat_L_Y, joystickHat_R_X);

  joystickHat_R_X = map(joystickHat_R_X, -127, 127, -MaxPower, MaxPower);
  if (motorPower < border)
    motorPower = 0;
  else if (motorPower >= border)
    motorPower = motorPower * sin(joystickAngle - motorAngle);

  if (abs(joystickHat_R_X ) > border)
    motorPower = motorPower + joystickHat_R_X / 2;

  motorPower = min(motorPower, MaxPower);
  motorPower = max(motorPower, -MaxPower);


  return motorPower;
}

void Omni::Run(int motorpower) {
  if (abs(motorpower) < border) {
    digitalWrite(pinA, HIGH);
    digitalWrite(pinB, HIGH);
    analogWrite(pinPwm, 0);
    Serial.println(0);
  } else if (motorpower > border) {
    digitalWrite(pinA, HIGH);
    digitalWrite(pinB, LOW);
    analogWrite(pinPwm, motorpower);
    Serial.println(motorpower);
  } else if (motorpower < -border) {
    digitalWrite(pinA, LOW);
    digitalWrite(pinB, HIGH);
    analogWrite(pinPwm, abs(motorpower));
    Serial.println(-motorPower);
  }
}

