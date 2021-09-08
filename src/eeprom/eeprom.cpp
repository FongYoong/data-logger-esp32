#include "eeprom.h"
#include "../unix_time/unix_time.h"
#include "../firebase/firebase.h"

Preferences preferences;

void EEPROMSetup() {
  preferences.begin("data-logger", false);
  demoMode = preferences.getBool("demoMode", true);
  enableLogging = preferences.getBool("enableLogging", true);
  logInterval = preferences.getULong("logInterval", 1000);
  temperatureLimit = preferences.getFloat("temperatureLimit", 25);
}

void updateConfigEEPROM() {
  preferences.putBool("demoMode", demoMode);
  preferences.putBool("enableLogging", enableLogging);
  preferences.putULong("logInterval", logInterval);
  preferences.putFloat("temperatureLimit", temperatureLimit);
}

void addOfflineLog(float temperature) {
  Serial.println("------------------------------------");
  Serial.println("Storing offline log");
  unsigned long unixTime = getUnixTime();
  Serial.println("Offline time: " + String(unixTime));
  Serial.println("Offline temperature: " + String(temperature));
  unsigned char offlineOffset = preferences.getUChar("offOffset", MIN_OFFLINE_LOGS_OFFSET);
  preferences.putFloat(("offLogTemp" + String(offlineOffset)).c_str(), temperature);
  preferences.putULong(("offLogTime" + String(offlineOffset)).c_str(), unixTime);
  preferences.putBool(("offLogPend" + String(offlineOffset)).c_str(), true);
  preferences.putUChar("offOffset", ((offlineOffset + 1 - MIN_OFFLINE_LOGS_OFFSET) % MAX_OFFLINE_LOGS) + MIN_OFFLINE_LOGS_OFFSET);
  bool pendingUpload = preferences.getBool(("offLogPend" + String(offlineOffset)).c_str(), false);
  Serial.println("\n------------------------------------");
}

void uploadOfflineLogs() {
  Serial.println("------------------------------------");
  Serial.println("Checking offline logs...");
  const unsigned int maxOffset = MIN_OFFLINE_LOGS_OFFSET + MAX_OFFLINE_LOGS;
  for (unsigned char offset = MIN_OFFLINE_LOGS_OFFSET; offset < maxOffset; offset++) {
    bool pendingUpload = preferences.getBool(("offLogPend" + String(offset)).c_str(), false);
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
