#include<Wire.h>
#include"LEDTape.h"

const int CONTROLLER_ADDR = 9;

LEDTape Plate_Coco(5, 3, 6), Plate_Choco(9, 10, 11);

bool SquareToggle, TriangleToggle;
bool SquareClick, TriangleClick;
bool sqr, tri;

struct buttonData{
  bool oldData = 0;
  bool newData = 0;
}Square, Triangle;

void setup() {
  Serial.begin(115200);
  Wire.begin(CONTROLLER_ADDR);
  Wire.onReceive(ReceiveEvent);
  Wire.setClock(400000UL); // Set I2C frequency to 400kHz

}

void loop() {

  if (SquareToggle)
    Plate_Coco.LightingSolid(255, 255, 0);
  else
    Plate_Coco.LightOff();

  if (TriangleToggle)
    Plate_Choco.LightingSolid(0, 0, 255);
  else
    Plate_Choco.LightOff();

}

void ReceiveEvent(int any) {

  while (Wire.available()) {

    sqr = Wire.read();
    tri = Wire.read();

  }

  Square.newData = sqr;

  if (Square.oldData == 0 && Square.newData == 1) {
    SquareClick = 1;
  }
  else
    SquareClick = 0;

  Square.oldData = Square.newData;

  if (SquareClick) SquareToggle ^= 1;
  

  Triangle.newData = tri;

  if (Triangle.oldData == 0 && Triangle.newData == 1) {
    TriangleClick = 1;
  }
  else
    TriangleClick = 0;

  Triangle.oldData = Triangle.newData;

  if (TriangleClick) TriangleToggle ^= 1;

}

