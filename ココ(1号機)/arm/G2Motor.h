#pragma once

const int DIR1 = 2;
const int DIR2 = 4;
const int DIR3 = 12;
const int DIR4 = 5;

const int PWM1 = 3;
const int PWM2 = 9;
const int PWM3 = 11;
const int PWM4 = 10;

class G2Motor {
  public:

    G2Motor() {
      pinMode(DIR1, OUTPUT);
      pinMode(DIR2, OUTPUT);
      pinMode(DIR3, OUTPUT);
      pinMode(DIR4, OUTPUT);

      pinMode(PWM1, OUTPUT);
      pinMode(PWM2, OUTPUT);
      pinMode(PWM3, OUTPUT);
      pinMode(PWM4, OUTPUT);
    }

    void Run(byte port, byte mode, byte PWM) {
      switch (port) {
        case 1:
          switch (mode) {
            case 0:
            case 3:
              digitalWrite(DIR1, LOW);
              analogWrite(PWM1, 0);
              break;
            case 1:
              digitalWrite(DIR1, HIGH);
              analogWrite(PWM1, PWM);
              break;
            case 2:
              digitalWrite(DIR1, LOW);
              analogWrite(PWM1, PWM);
              break;
          }
        case 2:
          switch (mode) {
            case 0:
            case 3:
              digitalWrite(DIR2, LOW);
              analogWrite(PWM2, 0);
              break;
            case 1:
              digitalWrite(DIR2, HIGH);
              analogWrite(PWM2, PWM);
              break;
            case 2:
              digitalWrite(DIR2, LOW);
              analogWrite(PWM2, PWM);
              break;
          }
        case 3:
          switch (mode) {
            case 0:
            case 3:
              digitalWrite(DIR3, LOW);
              analogWrite(PWM3, 0);
              break;
            case 1:
              digitalWrite(DIR3, HIGH);
              analogWrite(PWM3, PWM);
              break;
            case 2:
              digitalWrite(DIR3, LOW);
              analogWrite(PWM3, PWM);
              break;
          }
        case 4:
          switch (mode) {
            case 0:
            case 3:
              digitalWrite(DIR4, LOW);
              analogWrite(PWM4, 0);
              break;
            case 1:
              digitalWrite(DIR4, HIGH);
              analogWrite(PWM4, PWM);
              break;
            case 2:
              digitalWrite(DIR4, LOW);
              analogWrite(PWM4, PWM);
              break;
          }
      }
    }
     void SetPWMFrequency() {
      TCCR1B = (TCCR1B & 0b11111000) | 0x02;
      TCCR2B = (TCCR2B & 0b11111000) | 0x02;
    }


};

