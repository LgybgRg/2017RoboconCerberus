#include<Wire.h>
#include"G2Motor_LAF.h"
#include"i2cSlaveReceiver.h"

const int I2C_ADDRESS = 1;
const int CEASETIME_MS = 500;

G2Motor Motor12;

byte pwm1, pwm2, pwm3, pwm4;
unsigned long CeaseCounter;
bool flag;

void setup() {
  delay(2500);
  Wire.begin(I2C_ADDRESS);
  Wire.onReceive(ReceiveEvent);
  Serial.begin(115200);
  Motor12.SetPWMFrequency();
}

void loop() {

  if (flag) {

    CeaseCounter = millis();
    flag = false;
  }
  else {
    if ((millis() - CeaseCounter) > CEASETIME_MS) {
      pwm1 = pwm2 = pwm3 = pwm4 = 127;
    }
  }

  Motor12.Run(1, pwm1);
  Motor12.Run(2, pwm2);
  Motor12.Run(3, pwm3);
  Motor12.Run(4, pwm4);

  Show();
}

void ReceiveEvent(int hoge) {
  while (Wire.available()) {
    flag = true;
    pwm1 = Wire.read();
    pwm2 = Wire.read();
    pwm3 = Wire.read();
    pwm4 = Wire.read();
  }
}

void Show() {
  Serial.print("1:");  Serial.print(pwm1); Serial.print("\t");
  Serial.print("2:");  Serial.print(pwm2); Serial.print("\t");
  Serial.print("3:");  Serial.print(pwm3); Serial.print("\t");
  Serial.print("4:");  Serial.println(pwm4);
}
