#include"LEDTape.h"
#include<Wire.h>

const int CONTROLLER_ADDR = 10;

LEDTape Plate_Moco(9, 10, 11), Roof(5, 3, 6);

bool CircleToggle, CrossToggle;
bool CircleClick, CrossClick;
bool cir, crs;
double i_old;
uint8_t leftHatX, leftHatY, L2;
int interval = 15;

struct buttonData {
  bool oldData = 0;
  bool newData = 0;
} Circle, Cross;

void setup() {
  Serial.begin(115200);
  Wire.begin(CONTROLLER_ADDR);
  Wire.onReceive(ReceiveEvent);
  Wire.setClock(400000UL); // Set I2C frequency to 400kHz
}

void loop() {
  static double i;
  static unsigned long timer_ms;

  if (CircleToggle)
    Plate_Moco.LightingSolid(0, 255, 0);
  else
    Plate_Moco.LightOff();


  if (CrossClick) {
    CrossToggle == 1;
    timer_ms = millis();
  }
  else if (CrossToggle == 0) {

    Roof.LightingSolid(sin(i) * 100 , 0, sin(i) * 255);
    i += 0.00015;
  }
  else {
    if (millis() - timer_ms <= interval) {
      Roof.LightingSolid(255, 255, 255);
    }
    if (millis() - timer_ms > interval * 2 && millis() - timer_ms <= interval * 4) {
      Roof.LightingSolid(0, 0, 0);
    }
    if (millis() - timer_ms > interval * 4)
      timer_ms = millis();
  }


  if (i >= 3.14) i = 0;
}

void ReceiveEvent(int any) {

  while (Wire.available()) {

    cir = Wire.read();
    crs = Wire.read();

    leftHatX = Wire.read();
    leftHatY = Wire.read();
    L2 = Wire.read();

  }


  Circle.newData = cir;

  if (Circle.oldData == 0 && Circle.newData == 1) {
    CircleClick = 1;
  }
  else
    CircleClick = 0;

  Circle.oldData = Circle.newData;

  if (CircleClick) CircleToggle ^= 1;


  Cross.newData = crs;

  if (Cross.oldData == 0 && Cross.newData == 1) {
    CrossClick = 1;
  }
  else
    CrossClick = 0;

  Cross.oldData = Cross.newData;

  if (CrossClick) CrossToggle ^= 1;

}

