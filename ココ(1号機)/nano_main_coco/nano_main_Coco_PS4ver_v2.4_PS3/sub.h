#pragma once

//Gyro.cpp
int GetGyroOffset();
int GetGyro();

//Omni_Auto.cpp
int GetAngle_deg(long origin_x, long origin_y, long ref_x, long ref_y);
long GetDistance(long origin_x, long origin_y, long ref_x, long ref_y);
void auto_run();

//Arm.cpp
void arm();

//Solenoid.cpp
void solenoid();

//OmniCorrection_gyro.cpp
void Omni_gyroCorrection();

//encoderInitialize.cpp
void EncoderInitialize();

void Tire();

//ALLUPDATE.cpp
void ALLUPDATE();

struct buttonData {
  bool oldData = 0;
  bool newData = 0;
};

