#include<Wire.h>
#include"G2Motor.h"

const int I2C_ADDRESS = 1;

G2Motor Motor12;

byte mode1, mode2, mode3, mode4, pwm1, pwm2, pwm3, pwm4;
unsigned int long last = 0;
unsigned int long time = 0;

const int CEASETIME_MS = 1000;
unsigned long CeaseCounter;
bool flag;

void setup() {
  Wire.begin(I2C_ADDRESS);
  Wire.onReceive(ReceiveEvent);
  Serial.begin(115200);
  //Motor12.SetPWMFrequency();
}

void loop() {

  if (flag == 0) {
    if ((millis() - CeaseCounter) > CEASETIME_MS)
      pwm1 = pwm2 = pwm3 = pwm4 = 0;
  }
  else {
    CeaseCounter = millis();
    flag = false;
  }

  Motor12.Run(1, mode1, pwm1);
  Motor12.Run(2, mode2, pwm2);
  Motor12.Run(3, mode3, pwm3);
  Motor12.Run(4, mode4, pwm4);

}

void ReceiveEvent(int hoge) {
  while (Wire.available()) {

    flag = true;

    mode1 = Wire.read();
    pwm1 = Wire.read();
    mode2 = Wire.read();
    pwm2 = Wire.read();
    mode3 = Wire.read();
    pwm3 = Wire.read();
    mode4 = Wire.read();
    pwm4 = Wire.read();
  }
}

void show() {
  Serial.print(mode1); Serial.print("   ");
  Serial.print(mode2); Serial.print("   ");
  Serial.print(mode3); Serial.print("   ");
  Serial.print(mode4); Serial.print("   ");
  Serial.print(pwm1); Serial.print("   ");
  Serial.print(pwm2); Serial.print("   ");
  Serial.print(pwm3); Serial.print("   ");
  Serial.println(pwm4);
}
