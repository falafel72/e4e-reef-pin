
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

void set_digit(uint16_t value) 
{
  delay(200);
  uint16_t value_one = value%10;
  uint16_t value_ten = (value/10) % 10;
  uint16_t value_hun = (value/100) % 10;
  uint16_t value_tho = (value/1000) % 10;
  Wire.beginTransmission(0x38);
  Wire.write(0x20);
  Wire.write(d[value_one]);
  Wire.endTransmission();
  Wire.beginTransmission(0x38);
  Wire.write(0x21);
  Wire.write(d[value_ten]);
  Wire.endTransmission();
  Wire.beginTransmission(0x38);
  Wire.write(0x22);
  Wire.write(d[value_hun]);
  Wire.endTransmission();
  Wire.beginTransmission(0x38);
  Wire.write(0x23);
  Wire.write(d[value_tho]);
  Wire.endTransmission();
}

void loop()
{
  for(int i = 1000; i < 10000; i++){
    set_digit(i);
  }
}
