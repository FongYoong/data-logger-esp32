#define MDASH_APP_NAME "DataLogger"
#define MDASH_DEVICE_PASSWORD "fJk90L99Sy91G5WgYDO4Kg7qQ"
#include <mDash.h>

bool enableLogging = true;
float logInterval = 1000; // Milliseconds
float temperatureLimit = 25; // Celcius

#include "src/wifi_utils/wifi_utils.h"
#include "src/eeprom/eeprom.h"
#include "src/firebase/firebase.h"
#include "src/unix_time/unix_time.h"
#include "src/pins/pins.h"
#include "src/user_interface/user_interface.h"
#include "src/rtos/rtos.h"

void setup() {
  Serial.begin(115200);
  //disableCore0WDT();
  //disableCore1WDT();
  pinsSetup();
  EEPROMSetup();
  wifiSetup();
  mDashBegin(MDASH_DEVICE_PASSWORD);
  firebaseSetup();
  timeSetup();
  userInterfaceSetup();
  rtosSetup();
}

void loop() {
  
}
