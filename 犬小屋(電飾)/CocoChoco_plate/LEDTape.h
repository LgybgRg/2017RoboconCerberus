#pragma once
#include<arduino.h>

class  LEDTape
{
  private:
    const int _pinR, _pinG, _pinB;
    uint8_t val_r, val_g, val_b;
    unsigned long time_ms;

    void getRGB(uint8_t r, uint8_t g, uint8_t b) {
      val_r = r;
      val_g = g;
      val_b = b;
    }

  public:
    LEDTape(const int pinR, const int pinG, const int pinB);

    void LightingSolid(uint8_t r, uint8_t g, uint8_t b);
    void LightingBlink(uint8_t r, uint8_t g, uint8_t b);
    void LightOff();
    void LightingFade(uint8_t r, uint8_t g, uint8_t b);
    void ShowRGB();
    uint8_t GetR() {
      return val_r;
    }
    uint8_t GetG() {
      return val_g;
    }
    uint8_t GetB() {
      return val_b;
    }

};
