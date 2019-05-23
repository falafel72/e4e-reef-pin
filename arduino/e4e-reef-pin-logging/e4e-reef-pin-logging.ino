#include <LowPower.h>
#include <SPI.h>
#include <SD.h>
#include <Wire.h>
#include <SparkFun_MS5803_I2C.h>
#include <SoftwareSerial.h>

#define DEPTH_DISPLAY_DRIVER_ADDR 0x38
#define DECIMAL_PT_REG 0x24
#define SECOND_DECIMAL 0x40
#define SEA_LVL_PRESSURE 970
#define WATER_DENSITY 1029 

#define G 9.8

#define CHARGE_STAT_PIN 7
#define WET_DETECTOR_PIN 6
#define BLUETOOTH_RX 3
#define BLUETOOTH_TX 2

#define LOW_POWER_STATE 0
#define LOGGING_STATE 1
#define BLUETOOTH_STATE 2

byte digits[] = {0x7e, 0x30, 0x6d, 0x79, 0x33, 0x5b, 0x5f, 0x70, 0x7f, 0x7b};
double depth;
int display_depth;
int decimal_point;

// Begin class with selected address
// available addresses (selected by jumper on board) 
// default is ADDRESS_HIGH

//  ADDRESS_HIGH = 0x76
//  ADDRESS_LOW  = 0x77



int set_number(int number);
MS5803 sensor(ADDRESS_HIGH);
void set_digit(uint8_t displayRegOffset, uint8_t index);
void i2c_send_byte(uint8_t address, uint8_t instruction, uint8_t value);

//Create variables to store results
float temperature_c, temperature_f;
double pressure_abs, pressure_relative, altitude_delta, pressure_baseline;

// Create Variable to store altitude in (m) for calculations;
double base_altitude = 1655.0; // Altitude of SparkFun's HQ in Boulder, CO. in (m)

SoftwareSerial BT (BLUETOOTH_RX, BLUETOOTH_TX);

int state = 0;

void charging() {
  // charging interrupt
  // should change state variable
}

void not_charging() {
  // switches when charging stops
  // should change state variable
}

void in_water() {
  // triggers when in water
  // should change state variable
}

void i2c_send(uint8_t address, uint8_t instruction, uint8_t *data, int len) {
  Wire.beginTransmission(address);
  Wire.write(instruction);
  Wire.write(data,len);
  Wire.endTransmission();
}

void log_data() {
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

  // Let's do something interesting with our data.

  // Convert abs pressure with the help of altitude into relative pressure
  // This is used in Weather stations.
  pressure_relative = sealevel(pressure_abs, base_altitude);

  // Taking our baseline pressure at the beginning we can find an approximate
  // change in altitude based on the differences in pressure.   
  altitude_delta = altitude(pressure_abs , pressure_baseline);

  depth = (pressure_abs - SEA_LVL_PRESSURE)*100/(WATER_DENSITY * G);
  display_depth = (int) (depth * 10000);
  Serial.println(display_depth);
  set_number(display_depth);
  File dataFile = SD.open("data.txt",FILE_WRITE);

  if (dataFile) {
    char buf[10];
    char buf2[10];
    dtostrf(pressure_relative,4,2,buf);
    sprintf(buf2,"%s\n",buf);
    dataFile.write(buf2);
    Serial.print(buf2);
    dataFile.close();
  } else {
    Serial.println("error opening file");
  }

  delay(200);
}

void send_data() {
  // Should send current data.txt file from SD card over bluetooth.
}

void setup() {
  
  pinMode(CHARGE_STAT_PIN, INPUT_PULLUP);

  attachInterrupt(digitalPinToInterrupt(CHARGE_STAT_PIN), charging, FALLING);
  attachInterrupt(digitalPinToInterrupt(CHARGE_STAT_PIN), charging, RISING);
  attachInterrupt(digitalPinToInterrupt(WET_DETECTOR_PIN), in_water, RISING);
  
  Serial.begin(9600);
  //Retrieve calibration constants for conversion math.
  sensor.reset();
  sensor.begin();

  pressure_baseline = sensor.getPressure(ADC_4096);
  Serial.print("Initializing SD card...");

  if (!SD.begin()) {
    Serial.println("initialization failed!");
    while (1);
  }
  Serial.println("initialization done.");  
  Wire.begin();

  i2c_send_byte(DEPTH_DISPLAY_DRIVER_ADDR, 0x24, 0x20); 
}

void loop() {

  while (state) {
    // either log data or send data here
  }

  LowPower.powerDown(SLEEP_FOREVER, ADC_OFF, BOD_OFF);
}


// Thanks to Mike Grusin for letting me borrow the functions below from 
// the BMP180 example code. 

 double sealevel(double P, double A)
// Given a pressure P (mbar) taken at a specific altitude (meters),
// return the equivalent pressure (mbar) at sea level.
// This produces pressure readings that can be used for weather measurements.
{
  return(P/pow(1-(A/44330.0),5.255));
}


double altitude(double P, double P0)
// Given a pressure measurement P (mbar) and the pressure at a baseline P0 (mbar),
// return altitude (meters) above baseline.
{
  return(44330.0*(1-pow(P/P0,1/5.255)));
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
}

void set_digit(uint8_t displayRegOffset, uint8_t index) {
  i2c_send_byte(DEPTH_DISPLAY_DRIVER_ADDR, 0x20 + displayRegOffset, digits[index]);
}

void i2c_send_byte(uint8_t address, uint8_t instruction, uint8_t value) {
  Wire.beginTransmission(address);
  Wire.write(instruction);
  Wire.write(value);
  Wire.endTransmission();
}
