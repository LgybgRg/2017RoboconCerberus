#include<Servo.h>
#include<Wire.h>S

Servo EDF_A,EDF_B;

const int pin_A=3;
const int pin_B=9;

byte power;

void setup(){
   Serial.begin(115200);
   Wire.begin(4);
   Wire.onReceive(ReceiveEvent);

   pinMode(3,OUTPUT);
   pinMode(9,OUTPUT);
   
   EDF_A.attach(pin_A,0,2000);
   EDF_B.attach(pin_B,0,2000);
}

void loop(){
   Serial.println(power);
   //EDF_A.write(power);
   //EDF_B.write(power);
   if(power==1){
      EDF_A.write(180);
      EDF_B.write(180);
   }else{
      EDF_A.write(0);
      EDF_B.write(0);
   }
}

void ReceiveEvent(int any){
   while(Wire.available()){
      power=Wire.read();
   }
}

