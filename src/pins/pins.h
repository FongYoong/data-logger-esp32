#ifndef PINS_H
#define PINS_H

#include "Arduino.h"
#include "SSD1306Wire.h"
extern SSD1306Wire oledDisplay;

#include <Wire.h>
#include "DHT.h"
#define DHTPIN 32
#define DHTTYPE DHT11
extern DHT dht; // Temperature sensor

#define DEBOUNCE_DELAY 250 // Milliseconds // Debouncing interval after a button is pressed
const char button_left = 5;
const char button_right = 18;
const char button_enter = 19;
const char button_back = 4;

const char warning_LED = 2;

const char potmPin = 34; // Potentiometer

void pinsSetup();

#endif