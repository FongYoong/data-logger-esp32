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
  // Store config values in EEPROM
  preferences.putBool("demoMode", demoMode);
  preferences.putBool("enableLogging", enableLogging);
  preferences.putULong("logInterval", logInterval);
  preferences.putFloat("temperatureLimit", temperatureLimit);
}

void addOfflineLog(float temperature) {
  // Add offline log to EEPROM
  Serial.println("------------------------------------");
  Serial.println("Storing offline log");
  unsigned long unixTime = getUnixTime(); //  Get Unix timestamp
  Serial.println("Offline time: " + String(unixTime));
  Serial.println("Offline temperature: " + String(temperature));
  unsigned char offlineOffset = preferences.getUChar("offOffset", MIN_OFFLINE_LOGS_OFFSET); // Get current offset
  preferences.putFloat(("offLogTemp" + String(offlineOffset)).c_str(), temperature); // Store temperature value
  preferences.putULong(("offLogTime" + String(offlineOffset)).c_str(), unixTime); // Store Unix timestamp
  preferences.putBool(("offLogPend" + String(offlineOffset)).c_str(), true); // Store pending upload boolean as true
  preferences.putUChar("offOffset", ((offlineOffset + 1 - MIN_OFFLINE_LOGS_OFFSET) % MAX_OFFLINE_LOGS) + MIN_OFFLINE_LOGS_OFFSET); // Increment current offset
  Serial.println("\n------------------------------------");
}

void uploadOfflineLogs() {
  // Upload pending offline logs to Firebase
  Serial.println("------------------------------------");
  Serial.println("Checking offline logs...");
  const unsigned int maxOffset = MIN_OFFLINE_LOGS_OFFSET + MAX_OFFLINE_LOGS; // Get max offset
  for (unsigned char offset = MIN_OFFLINE_LOGS_OFFSET; offset < maxOffset; offset++) {
    // Loop through each location from min offset to max offset
    bool pendingUpload = preferences.getBool(("offLogPend" + String(offset)).c_str(), false); // Check whether pending upload
    if (pendingUpload) {
      Serial.println("Uploading log " + String(offset));
      float temperature = preferences.getFloat(("offLogTemp" + String(offset)).c_str(), 0); // Get temperature value
      unsigned long timestamp = preferences.getULong(("offLogTime" + String(offset)).c_str(), 0); // Get Unix timestamp
      addLogtoFirebase(temperature, timestamp); // Upload log to Firebase
      preferences.putBool(("offLogPend" + String(offset)).c_str(), false); // Set pending upload boolean to false
    }
  }
  Serial.println("------------------------------------");
}
