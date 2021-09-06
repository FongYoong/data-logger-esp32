#include "eeprom.h"
#include "../unix_time/unix_time.h"
#include "../firebase/firebase.h"

Preferences preferences;

void EEPROMSetup() {
  preferences.begin("data-logger", false);
  enableLogging = preferences.getBool("enableLogging", true);
  logInterval = preferences.getFloat("logInterval", 1000);
  temperatureLimit = preferences.getFloat("temperatureLimit", 25);
}

void updateConfigEEPROM() {
  preferences.putBool("enableLogging", enableLogging);
  preferences.putFloat("logInterval", logInterval);
  preferences.putFloat("temperatureLimit", temperatureLimit);
}

void addOfflineLog(float temperature) {
  Serial.println("------------------------------------");
  Serial.println("Storing offline log");
  unsigned long unixTime = getUnixTime();
  Serial.println("Offline time: " + String(unixTime));
  Serial.println("Offline temperature: " + String(temperature));
  unsigned char offlineOffset = preferences.getUChar("offOffset", 0);
  preferences.putFloat(("offLogTemp" + String(offlineOffset)).c_str(), temperature);
  preferences.putULong(("offLogTime" + String(offlineOffset)).c_str(), unixTime);
  preferences.putBool(("offLogPend" + String(offlineOffset)).c_str(), true);
  preferences.putUChar("offOffset", (offlineOffset + 1) % MAX_OFFLINE_LOGS);
  //Serial.println("offlineOffset: " + String(offlineOffset));
  bool pendingUpload = preferences.getBool(("offLogPend" + String(offlineOffset)).c_str(), false);
  //Serial.println("Set PEND: " + String(pendingUpload));
  Serial.println("\n------------------------------------");
}

void uploadOfflineLogs() {
  Serial.println("------------------------------------");
  Serial.println("Checking offline logs...");
  for (unsigned char offset = 0; offset < MAX_OFFLINE_LOGS; offset++) {
    //Serial.println("offlineOffset: " + String(offset));
    //Serial.println("name: offLogPend" + String(offset));
    bool pendingUpload = preferences.getBool(("offLogPend" + String(offset)).c_str(), false);
    //Serial.println("Check PEND: " + String(pendingUpload));
    if (pendingUpload) {
      Serial.println("Uploading log " + String(offset));
      float temperature = preferences.getFloat(("offLogTemp" + String(offset)).c_str(), 0);
      unsigned long timestamp = preferences.getULong(("offLogTime" + String(offset)).c_str(), 0);
      addLogtoFirebase(temperature, timestamp);
      preferences.putBool(("offLogPend" + String(offset)).c_str(), false);
    }
  }
  Serial.println("------------------------------------");
}
