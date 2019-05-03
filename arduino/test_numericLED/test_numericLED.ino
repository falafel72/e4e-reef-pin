
#include <Wire.h>
byte d[] = {0x7e, 0x30, 0x6d, 0x79, 0x33, 0x5b, 0x5f, 0x70, 0x7f, 0x7b };
void setup()
{
  Wire.begin();
  Wire.beginTransmission(0x38);
  Wire.write(0x01); // register decode mode
  Wire.write(0x00); // disable decode mode for all digits
  Wire.write(0x3f); // intensity max
  Wire.write(0x03); // scan limit 3
  Wire.write(0x03); // normal operation
  Wire.endTransmission();
}

void set_digit(byte digit, byte value) 
{
  Wire.beginTransmission(0x38);
  Wire.write(0x20 + digit);
  Wire.write(d[value]);
  Wire.endTransmission();
}

void loop()
{
  for(int i = 0; i < 10; i++)
  {
    set_digit(2, i);
    set_digit(1, i);
    set_digit(0, i);
    delay(1000);
  }
//  set_digit(2, 8);
//  set_digit(1, 8);
//  set_digit(0, 8);
  
}
