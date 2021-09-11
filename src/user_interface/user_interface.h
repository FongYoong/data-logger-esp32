#ifndef USER_INTERFACE_H
#define USER_INTERFACE_H

#include "Arduino.h"

enum UserAction {
  UA_NONE,
  UA_ENTER,
  UA_BACK,
  UA_LEFT,
  UA_RIGHT
}; // All user actions

enum UIPage {
  P_HOME,
  P_EDIT_LOG_INTERVAL,
  P_EDIT_TEMPERATURE_LIMIT
}; // All UI pages

enum HomeOption {
    HO_ENABLE_LOGGING = 0,
    HO_EDIT_LOG_INTERVAL = 1,
    HO_EDIT_TEMPERATURE_LIMIT = 2
}; // All home states

// Access global variables
extern bool enableLogging;
extern unsigned long logInterval;
extern float temperatureLimit;
extern float temperatureValue;

#define logIntervalStep 50 // Milliseconds // Increase/decrease step
#define minLogInterval 500 // Milliseconds // Minimum log interval
#define temperatureLimitStep 0.1 // Celcius // Increase/decrease step
#define minTemperatureLimit -273.0 // Celcius // Minimum temperature limit

void userInterfaceSetup();
void processInputs();
void renderUI();
void displayHome();
void displayEditLogInterval();
void displayEditTemperatureLimit();

#endif