#pragma once

int GetGyro();
int GetGyroOffset();

void edf();

struct buttonData {
  bool oldData = 0;
  bool newData = 0;
};

