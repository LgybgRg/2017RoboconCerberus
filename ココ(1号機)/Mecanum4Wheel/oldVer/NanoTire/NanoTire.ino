#include <Wire.h>
#include"MovingAverage.h"
#include "Omni.h"

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

const int AverageSize = 100;

Omni motorFR(M1A , M1B , M1PWM , PI / 4, false); //1
Omni motorBR(M2A , M2B , M2PWM , PI * 7 / 4, false); //2
Omni motorBL(M3A , M3B , M3PWM , PI * 5 / 4, false); //3
Omni motorFL(M4A , M4B , M4PWM, PI * 3 / 4, false); //4

int leftHatX, leftHatY, rightHatX;

int power1, power2, power3, power4;

MovingAverage average1(AverageSize);
MovingAverage average2(AverageSize);
MovingAverage average4(AverageSize);

void setup() {
  delay(2000);
  Serial.begin(115200);
  Wire.begin(10);
  Wire.onReceive(REvent);
  pinMode(M1A, OUTPUT);
  pinMode(M1B, OUTPUT);
  pinMode(M2A, OUTPUT);
  pinMode(M2B, OUTPUT);
  pinMode(M3A, OUTPUT);
  pinMode(M3B, OUTPUT);
  pinMode(M4A, OUTPUT);
  pinMode(M4B, OUTPUT);

}

void loop() {

  int rightHat_x =  map(rightHatX, 0, 255, -255, 255);
  int leftHat_x =  -map(leftHatX, 0, 255, -255, 255);
  int leftHat_y =  -map(leftHatY, 0, 255, -255, 255);

  power1 = motorFR.drive(leftHat_x, leftHat_y, rightHat_x);
  power2 = motorBR.drive(leftHat_x, leftHat_y, rightHat_x);
  power3 = motorBL.drive(leftHat_x, leftHat_y, rightHat_x);
  power4 = motorFL.drive(leftHat_x, leftHat_y, rightHat_x);

  motorFR.Run(power1);
  motorBR.Run(power2);
  motorBL.Run(power3);
  motorFL.Run(power4);

  /*Serial.print(motorFR.drive(leftHat_x, leftHat_y, rightHat_x));
    Serial.print("\t");
    Serial.print(motorBR.drive(leftHat_x, leftHat_y, rightHat_x));
    Serial.print("\t");
    Serial.print(motorBL.drive(leftHat_x, leftHat_y, rightHat_x));
    Serial.print("\t");
    Serial.print(motorFL.drive(leftHat_x, leftHat_y, rightHat_x));
    Serial.println("\t");
  */

}

void REvent(int Many) {
  while (Wire.available()) {
    leftHatX  = Wire.read();
    leftHatY  = Wire.read();
    rightHatX  = Wire.read();
  }
}
