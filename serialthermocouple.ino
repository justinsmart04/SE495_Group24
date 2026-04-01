/***************************************************
  This is an example for the Adafruit Thermocouple Sensor w/MAX31855K

  Designed specifically to work with the Adafruit Thermocouple Sensor
  ----> https://www.adafruit.com/products/269

  These displays use SPI to communicate, 3 pins are required to
  interface
  Adafruit invests time and resources providing this open source code,
  please support Adafruit and open-source hardware by purchasing
  products from Adafruit!

  Written by Limor Fried/Ladyada for Adafruit Industries.
  BSD license, all text above must be included in any redistribution
 ****************************************************/

#include <SPI.h>
#include "Adafruit_MAX31855.h"

// Default connection is using software SPI, but comment and uncomment one of
// the two examples below to switch between software SPI and hardware SPI:

// Example creating a thermocouple instance with software SPI on any three
// digital IO pins.
// #define MAXDO   3
// #define MAXCS   4
// #define MAXCLK  5
#define MAXDO   12
#define MAXCS   10
#define CS2     11 // this will be thermocouple 2 
#define MAXCLK  13
// initialize the Thermocouple
 Adafruit_MAX31855 thermo1(MAXCLK, MAXCS, MAXDO);
 Adafruit_MAX31855 thermo2(MAXCLK, CS2, MAXDO);


// Example creating a thermocouple instance with hardware SPI
// on a given CS pin.
// #define MAXCS   10
// Adafruit_MAX31855 thermocouple(MAXCS);

void setup() {
  Serial.begin(9600);
  while (!Serial) delay(1);

  Serial.println("MAX31855 test");
  delay(500);

  thermo1.begin();
  thermo2.begin();
}

void loop() {

  double t1 = thermo1.readCelsius();
  double t2 = thermo2.readCelsius();

  Serial.print("TC1: ");
  Serial.print(t1);
  Serial.print(" C   ");

  Serial.print("TC2: ");
  Serial.print(t2);
  Serial.println(" C");

  delay(1000);
}