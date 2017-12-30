#pragma once

class valve {
  private:
    int buttonData, pin;

  public:
    valve(int PIN) {
      pinMode(PIN, OUTPUT);
      pin = PIN;
    }

    void valveRun(int DATA) {
      buttonData = DATA;
      if (buttonData) {
        digitalWrite(pin, HIGH);
      } else {
        digitalWrite(pin, LOW);
      }
    }

    int showData() {
      Serial.print(buttonData);  Serial.print("\t");
    }

    void Update(){
      
    }
};

