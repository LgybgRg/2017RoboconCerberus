#pragma once

class i2cSlaveReceiver {
  private:

    const int i2c_address;
    byte *data;
    int Size;

  public:

    i2cSlaveReceiver(const int address, int array_size): i2c_address(address) {
      data = new byte[array_size];
      Size=array_size;
      
      for (int i = 0; i < array_size; i++) {
        data[i] = 0;
      }
    }

    virtual ~i2cSlaveReceiver(){
      delete[] data;
    }

    void Update(){
      while(Wire.available()>=Size){
         for(int i=0;i<Size;i++)
         data[i]=Wire.read();
      }
    }

   void Show(){
      for(int i=0;i<Size;i++){
         Serial.print(data[i]); Serial.print("\t");
      }
      Serial.println("");
   }
};

