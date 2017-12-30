#include "LEDTape.h"

LEDTape::LEDTape(const int pinR, const int pinG, const int pinB): _pinR(pinR), _pinG(pinG), _pinB(pinB) {
  pinMode(pinR, OUTPUT);
  pinMode(pinG, OUTPUT);
  pinMode(pinB, OUTPUT);
}

void LEDTape::LightingSolid(uint8_t r, uint8_t g, uint8_t b) {
  analogWrite(_pinR, r);
  analogWrite(_pinG, g);
  analogWrite(_pinB, b);

  getRGB(r, g, b);
}

void LEDTape::LightingBlink(uint8_t r, uint8_t g, uint8_t b) {
  time_ms = millis();
  while (millis() - time_ms <= 1000) {
    LightingSolid(r, g, b);
  }

  while (millis() - time_ms <= 2000)
    LightingSolid(0, 0, 0);
  Serial.print(millis() - time_ms);

  getRGB(r, g, b);
}

void LEDTape::LightOff() {
  LightingSolid(0, 0, 0);
  getRGB(0, 0, 0);
}

void LEDTape::LightingFade(uint8_t r, uint8_t g, uint8_t b) {
}

void LEDTape::ShowRGB() {
  Serial.print(val_r);    Serial.print("\t");
  Serial.print(val_g);    Serial.print("\t");
  Serial.println(val_b);
}

