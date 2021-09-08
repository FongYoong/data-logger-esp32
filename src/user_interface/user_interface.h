#ifndef USER_INTERFACE_H
#define USER_INTERFACE_H

#include "Arduino.h"

enum UIPage {
  P_HOME,
  P_EDIT_LOG_INTERVAL,
  P_EDIT_TEMPERATURE_LIMIT
}; // represent UI page state

enum HomeOption {
    HO_ENABLE_LOGGING = 0,
    HO_EDIT_LOG_INTERVAL = 1,
    HO_EDIT_TEMPERATURE_LIMIT = 2
}; // represent home state

enum UserAction {
  UA_NONE, UA_ENTER, UA_BACK, UA_LEFT, UA_RIGHT
}; // represent user action

extern bool enableLogging;
extern unsigned long logInterval;
extern float temperatureLimit;
extern float temperatureValue;

#define logIntervalStep 50 //milliseconds
#define minLogInterval 500
#define temperatureLimitStep 0.1
#define minTemperatureLimit -273.0
//#define INPUT_CHECK_INTERVAL 500 // Milliseconds

void userInterfaceSetup();
void processInputs();
void renderUI();
void displayHome();
void displayEditLogInterval();
void displayEditTemperatureLimit();

#endif