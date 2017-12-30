#include <Wire.h>

/*
  Example sketch for the PS3 Bluetooth library - developed by Kristian Lauszus
  For more information visit my blog: http://blog.tkjelectronics.dk/ or
  send me an e-mail:  kristianl@tkjelectronics.com
*/

#include <PS3BT.h>
#include <usbhub.h>

// Satisfy the IDE, which needs to see the include statment in the ino too.
#ifdef dobogusinclude
#include <spi4teensy3.h>
#include <SPI.h>
#endif

#define DEBUG

USB Usb;
//USBHub Hub1(&Usb); // Some dongles have a hub inside

BTD Btd(&Usb); // You have to create the Bluetooth Dongle instance like so
/* You can create the instance of the class in two ways */
PS3BT PS3(&Btd); // This will just create the instance
//PS3BT PS3(&Btd, 0x00, 0x15, 0x83, 0x3D, 0x0A, 0x57); // This will also store the bluetooth address - this can be obtained from the dongle when running the sketch

uint8_t PAD_DATA[] = {0, 0, 127, 127, 127, 127, 0, 0, 1};
static int StartCount;

void setup()
{
  Serial.begin(115200);
#if !defined(__MIPSEL__)
  while (!Serial); // Wait for serial port to connect - used on Leonardo, Teensy and other boards with built-in USB CDC serial connection
#endif
  if (Usb.Init() == -1) {
    Serial.print(F("\r\nOSC did not start"));
    while (1); //halt
  }
  Serial.print(F("\r\nPS3 Bluetooth Library Started"));
  Wire.onRequest(requestEvent);
  Wire.begin(9);
  //Wire.setClock(400000UL); // Set I2C frequency to 400kHz

}

void loop()
{
  uint8_t PS3_DATA[] = {0, 0, 127, 127, 127, 127, 0, 0, 1};
  BTControlGet(PS3_DATA);
  for ( int i = 0; i < 9; i++ ) {
    PAD_DATA[i] = PS3_DATA[i];
  }

#ifdef DEBUG
  Serial.print(PS3_DATA[0], BIN);
  Serial.print("\t");
  Serial.print(PS3_DATA[1], BIN);
  Serial.print("\t");
  Serial.print(PS3_DATA[2]);
  Serial.print("\t");
  Serial.print(PS3_DATA[3]);
  Serial.print("\t");
  Serial.print(PS3_DATA[4]);
  Serial.print("\t");
  Serial.print(PS3_DATA[5]);
  Serial.print("\t");
  Serial.print(PS3_DATA[6]);
  Serial.print("\t");
  Serial.print(PS3_DATA[7]);
  Serial.print("\t");
  Serial.println(PS3_DATA[8]);
#endif

}


void BTControlGet(uint8_t data[])
{
  Usb.Task();
  bitWrite(data[8], 0, 1);
  if (PS3.PS3Connected || PS3.PS3NavigationConnected) {
    data[2] = PS3.getAnalogHat(RightHatX);
    data[3] = PS3.getAnalogHat(RightHatY);
    data[4] = PS3.getAnalogHat(LeftHatX);
    data[5] = PS3.getAnalogHat(LeftHatY);
    data[6] = PS3.getAnalogButton(L2);
    data[7] = PS3.getAnalogButton(R2);

    if (PS3.getButtonClick(PS)) {
      Serial.print(F("\r\nPS"));
      PS3.disconnect();
    } else {
      bitWrite(data[0], 0, PS3.getButtonPress(SELECT));
      bitWrite(data[0], 1, PS3.getButtonPress(L3));
      bitWrite(data[0], 2, PS3.getButtonPress(R3));
      bitWrite(data[0], 3, PS3.getButtonClick(START));
      bitWrite(data[0], 4, PS3.getButtonClick(UP));
      bitWrite(data[0], 5, PS3.getButtonClick(RIGHT));
      bitWrite(data[0], 6, PS3.getButtonClick(DOWN));
      bitWrite(data[0], 7, PS3.getButtonClick(LEFT));

      uint8_t buf1 = data[6] != 0 ? 1 : 0;
      uint8_t buf2 = data[7] != 0 ? 1 : 0;

      bitWrite(data[1], 0, buf1);
      bitWrite(data[1], 1, buf2);
      bitWrite(data[1], 2, PS3.getButtonPress(L1));
      bitWrite(data[1], 3, PS3.getButtonPress(R1));
      bitWrite(data[1], 4, PS3.getButtonClick(TRIANGLE));
      bitWrite(data[1], 5, PS3.getButtonClick(CIRCLE));
      bitWrite(data[1], 6, PS3.getButtonClick(CROSS));
      bitWrite(data[1], 7, PS3.getButtonClick(SQUARE));

      bitWrite(data[8], 0, PS3.getButtonPress(PS));

    }
  }
}

void requestEvent()
{
  Wire.write(PAD_DATA, 9);
}

