#pragma once

class timer {

  private:


  public:
    unsigned long int GetElapsedTime();
    bool isAvailableInterval(int target_time);
    void PrintActualCycle();

};

unsigned long int timer::GetElapsedTime() {
  static int current_time = millis();

  return (millis() - current_time);
}

bool timer::isAvailableInterval(int target_time) {
  static int current_time = millis();

  return (millis() - current_time) <= target_time ? 1 : 0;
}

void timer::PrintActualCycle() {
  static unsigned long lastTime = 0;
  unsigned long nowTime = micros();

  Serial.print("actualCycle : ");
  Serial.println( nowTime - lastTime );
  lastTime = nowTime;
}
