#include<Wire.h>
#include"Valve.h"

const int i2cAddress = 3;
int buttonCircle, buttonCross, buttonToriangle, buttoSquare, buttoS_Toriangle;
bool data[5];

valve valve1(12), valve2(8), valve3(3), valve4(2), valve5(4);

void setup() {
  Wire.begin(i2cAddress);
  Wire.onReceive(receiveEvent);
  Serial.begin(115200);
}

void loop() {
  valve1.valveRun(buttonCircle);
  valve2.valveRun(buttonCross);
  valve3.valveRun(buttonToriangle);
  valve4.valveRun(buttoSquare);
  valve5.valveRun(buttoS_Toriangle);

  show();
}

void receiveEvent(int howMany) {
  while (5 <= Wire.available()) {
    buttonCircle     = Wire.read();
    buttonCross      = Wire.read();
    buttonToriangle  = Wire.read();
    buttoSquare      = Wire.read();
    buttoS_Toriangle = Wire.read();
  }
}
void show(){
  valve1.showData();
  valve2.showData();
  valve3.showData();
  valve4.showData();
  valve5.showData();
  Serial.println();
}

