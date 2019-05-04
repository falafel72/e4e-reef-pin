#include <SPI.h>
#include <SD.h>
#include <Wire.h>
#include <SparkFun_MS5803_I2C.h>

#define DEPTH_DISPLAY_DRIVER_ADDR 0x38
#define DECIMAL_PT_REG 0x24
#define SECOND_DECIMAL 0x40

#define SEA_LVL_PRESSURE 1013.25
#define WATER_DENSITY 1029 
#define G 9.8

// Begin class with selected address
// available addresses (selected by jumper on board) 
// default is ADDRESS_HIGH

//  ADDRESS_HIGH = 0x76
//  ADDRESS_LOW  = 0x77

MS5803 sensor(ADDRESS_HIGH);

byte digits[] = {0x7e, 0x30, 0x6d, 0x79, 0x33, 0x5b, 0x5f, 0x70, 0x7f, 0x7b};

//Create variables to store results
float temperature_c, temperature_f;
double pressure_abs, pressure_relative, altitude_delta, pressure_baseline;

double depth;
int display_depth;
int decimal_point;

// Create Variable to store altitude in (m) for calculations;
double base_altitude = 1655.0; // Altitude of SparkFun's HQ in Boulder, CO. in (m)
double sea_level_pressure = 1013.25; // sea level pressure is 1013.25mb


void i2c_send_byte(uint8_t address, uint8_t instruction, uint8_t value) {
  Wire.beginTransmission(address);
  Wire.write(instruction);
  Wire.write(value);
  Wire.endTransmission();
}

void i2c_send_bytes(uint8_t address, uint8_t instruction, uint8_t *data, int len) {
  Wire.beginTransmission(address);
  Wire.write(instruction);
  Wire.write(data,len);
  Wire.endTransmission();
}

void set_digit(uint8_t displayRegOffset, uint8_t index) {
  i2c_send_byte(DEPTH_DISPLAY_DRIVER_ADDR, 0x20 + displayRegOffset, digits[index]);
}

int set_number(int number) {
  if (number < 0 || number > 9999) {
    return -1;
  }
  int unit = number % 10;
  number /= 10;
  int tens = number % 10;
  number /= 10;
  int hundreds = number % 10; 
  number /= 10;
  int thousands = number % 10;

  set_digit(3,unit);
  set_digit(2,tens);
  set_digit(1,hundreds);
  set_digit(0,thousands);

  Wire.beginTransmission(DEPTH_DISPLAY_DRIVER_ADDR);
  Wire.write(DECIMAL_PT_REG);
  Wire.write(SECOND_DECIMAL);
  Wire.endTransmission();
}

void initDepthDisplay() {
  Wire.beginTransmission(DEPTH_DISPLAY_DRIVER_ADDR);
  Wire.write(0x01); // register decode mode
  Wire.write(0x00); // disable decode mode for all digits
  Wire.write(0x3f); // intensity max
  Wire.write(0x03); // scan limit 3
  Wire.write(0x03); // normal operation
  Wire.endTransmission();
}

void setup() {
  Serial.begin(9600);
  //Retrieve calibration constants for conversion math.
  sensor.reset();
  sensor.begin();
    
  pressure_baseline = sensor.getPressure(ADC_4096);
  Serial.print("Initializing SD card...");

  if (!SD.begin(4)) {
    Serial.println("initialization failed!");
    while (1);
  }
  Serial.println("initialization done.");  
  Wire.begin();
  initDepthDisplay();
}

void loop() {
  
  // To measure to higher degrees of precision use the following sensor settings:
  // ADC_256 
  // ADC_512 
  // ADC_1024
  // ADC_2048
  // ADC_4096
    
  // Read temperature from the sensor in deg C. This operation takes about 
  temperature_c = sensor.getTemperature(CELSIUS, ADC_512);
  
  // Read temperature from the sensor in deg F. Converting
  // to Fahrenheit is not internal to the sensor.
  // Additional math is done to convert a Celsius reading.
  temperature_f = sensor.getTemperature(FAHRENHEIT, ADC_512);
  
  // Read pressure from the sensor in mbar.
  pressure_abs = sensor.getPressure(ADC_4096);
    
  depth = (pressure_abs - SEA_LVL_PRESSURE)*100/(WATER_DENSITY * G);
  display_depth = (int) (depth * 100);

  set_number(display_depth);

  File dataFile = SD.open("data.txt",FILE_WRITE);

  if (dataFile) {
    char buf[10];
    char buf2[10];
    dtostrf(depth,4,2,buf);
    sprintf(buf2,"%s\n",buf);
    dataFile.write(buf2);
    Serial.print(buf2);
    dataFile.close();
  } else {
    Serial.println("error opening file");
  }

  delay(200);

}
