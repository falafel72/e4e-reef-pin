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

void set_digit(uint16_t value_one) 
{
  delay(100);
  uint16_t value_ten = value_one/10;
  uint16_t value_hun = value_ten/10;
  uint16_t value_tho = value_hun/10;
  Wire.beginTransmission(0x38);
  Wire.write(0x20);
  Wire.write(d[1]);
  Wire.endTransmission();
  Wire.beginTransmission(0x38);
  Wire.write(0x21);
  Wire.write(d[2]);
  Wire.endTransmission();
  Wire.beginTransmission(0x38);
  Wire.write(0x22);
  Wire.write(d[3]);
  Wire.endTransmission();
  Wire.beginTransmission(0x38);
  Wire.write(0x23);
  Wire.write(d[4]);
  Wire.endTransmission();
}

void loop()
{
  set_digit(1234);
}
