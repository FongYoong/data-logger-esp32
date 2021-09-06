#ifndef PINS_H
  #define PINS_H

#include "Arduino.h"
#include "DHT.h"
#include <Wire.h>
//#include "SSD1306.h"
#include "SSD1306Wire.h"

//#define DHTPIN 23
//#define DHTTYPE DHT11
//SSD1306  display(0x3c, 21, 22);
SSD1306Wire display(0x3c, SDA, SCL);
//DHT dht(DHTPIN, DHTTYPE);

const char potmPin = 34;    // Potentiometer

#endif