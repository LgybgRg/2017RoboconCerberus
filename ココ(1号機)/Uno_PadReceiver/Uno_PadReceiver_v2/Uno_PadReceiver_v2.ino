#include <PS4BT.h>
#include <usbhub.h>
#include<Wire.h>
// Satisfy the IDE, which needs to see the include statment in the ino too.
#ifdef dobogusinclude
#include <spi4teensy3.h>
#include <SPI.h>
#endif

#define DEBUGoff

USB Usb;
USBHub Hub1(&Usb); // Some dongles have a hub inside
BTD Btd(&Usb); // You have to create the Bluetooth Dongle instance like so

/* You can create the instance of the PS4BT class in two ways */
// This will start an inquiry and then pair with the PS4 controller - you only have to do this once
// You will need to hold down the PS and Share button at the same time, the PS4 controller will then start to blink rapidly indicating that it is in paring mode
//PS4BT PS4(&Btd, PAIR);

// After that you can simply create the instance like so and then press the PS button on the device
PS4BT PS4(&Btd);

uint8_t PAD_DATA[] = {0, 0, 127, 127, 127, 127, 0, 0, 0};
static int StartCount, CrossCount;
static bool SoldData, SnewData, CrsoldData, CrsnewData, PidColor;
static int ColorSelector;
void setup() {
  Wire.begin(9);
  Wire.onRequest(requestEvent);
  Serial.begin(115200);
  Wire.setClock(400000UL); // Set I2C frequency to 400kHz

#if !defined(__MIPSEL__)
  while (!Serial); // Wait for serial port to connect - used on Leonardo, Teensy and other boards with built-in USB CDC serial connection
#endif
  if (Usb.Init() == -1) {
    Serial.print(F("\r\nOSC did not start"));
    while (1); // Halt
  }
  Serial.print(F("\r\nPS4 Bluetooth Library Started"));
}

void loop() {
  uint8_t PS4_DATA[] = {0, 0, 127, 127, 127, 127, 0, 0 , 0};
  BTControlGet(PS4_DATA);
  for ( int i = 0; i < 9; i++ ) {
    PAD_DATA[i] = PS4_DATA[i];
  }

#ifdef DEBUG
  Serial.print(PS4_DATA[0], BIN);
  Serial.print("\t");
  Serial.print(PS4_DATA[1], BIN);
  Serial.print("\t");
  Serial.print(PS4_DATA[2]);
  Serial.print("\t");
  Serial.print(PS4_DATA[3]);
  Serial.print("\t");
  Serial.print(PS4_DATA[4]);
  Serial.print("\t");
  Serial.print(PS4_DATA[5]);
  Serial.print("\t");
  Serial.print(PS4_DATA[6]);
  Serial.print("\t");
  Serial.println(PS4_DATA[7]);
#endif

}

void BTControlGet(uint8_t data[])
{
  Usb.Task();
  bitWrite(data[8], 0, 1);  //コントローラ未接続 - > PSボタン = 1
  if (PS4.connected()) {
    bitWrite(data[8], 0, 0); //PSボタン=0
    /*      Analog      */
    data[2] = PS4.getAnalogHat(RightHatX);
    data[3] = PS4.getAnalogHat(RightHatY);
    data[4] = PS4.getAnalogHat(LeftHatX);
    data[5] = PS4.getAnalogHat(LeftHatY);
    data[6] = PS4.getAnalogButton(L2);
    data[7] = PS4.getAnalogButton(R2);

    if (PS4.getButtonClick(PS)) {
      PS4.disconnect();
    } else {
      bitWrite(data[0], 0, PS4.getButtonPress(SHARE));
      bitWrite(data[0], 1, PS4.getButtonPress(L3));
      bitWrite(data[0], 2, PS4.getButtonPress(R3));
      bitWrite(data[0], 3, PS4.getButtonPress(START));
      bitWrite(data[0], 4, PS4.getButtonPress(UP));
      bitWrite(data[0], 5, PS4.getButtonPress(RIGHT));
      bitWrite(data[0], 6, PS4.getButtonPress(DOWN));
      bitWrite(data[0], 7, PS4.getButtonPress(LEFT));

      uint8_t buf1 = data[6] != 0 ? 1 : 0;
      uint8_t buf2 = data[7] != 0 ? 1 : 0;

      bitWrite(data[1], 0, buf1);
      bitWrite(data[1], 1, buf2);
      bitWrite(data[1], 2, PS4.getButtonPress(L1));
      bitWrite(data[1], 3, PS4.getButtonPress(R1));
      bitWrite(data[1], 4, PS4.getButtonPress(TRIANGLE));
      bitWrite(data[1], 5, PS4.getButtonPress(CIRCLE));
      bitWrite(data[1], 6, PS4.getButtonPress(CROSS));
      bitWrite(data[1], 7, PS4.getButtonPress(SQUARE));

      bitWrite(data[8], 1, PS4.getButtonPress(TOUCHPAD));

      //コントローラのLED設定
      SnewData = PS4.getButtonPress(START);

      if (SoldData == 0 && SnewData == 1)
        StartCount++;

      SoldData = SnewData;

      if (StartCount % 2 == 0)
        PS4.setLed(Blue);
      else if (StartCount % 2 == 1)
        PS4.setLed(Red);

    }
  }
}

void requestEvent()
{
  Wire.write(PAD_DATA, 9);
}

