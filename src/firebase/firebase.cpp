#include "firebase.h"
#include "../eeprom/eeprom.h"
#include "addons/TokenHelper.h" //Provide the token generation process info.
#include "addons/RTDBHelper.h" //Provide the RTDB payload printing info and other helper functions.

FirebaseAuth auth; // The user UID can be obtained from auth.token.uid
FirebaseConfig config; // FirebaseConfig data for config data
FirebaseData configFirebaseData;
FirebaseData loggingFirebaseData;

void firebaseSetup() {
  Serial.printf("Initializing Firebase v%s\n\n", FIREBASE_CLIENT_VERSION);
  config.api_key = API_KEY;
  auth.user.email = USER_EMAIL;
  auth.user.password = USER_PASSWORD;
  config.database_url = DATABASE_URL;
  Firebase.reconnectWiFi(true);
  config.token_status_callback = tokenStatusCallback; //Assign the callback function for the long running token generation task, see addons/TokenHelper.h
  config.max_token_generation_retry = 5; // Assign the maximum retry of token generation
  Firebase.begin(&config, &auth); // Initialize the library with the Firebase authen and config
  configFirebaseData.setResponseSize(4096);
  if (!Firebase.beginStream(configFirebaseData, "/config"))
  {
    Serial.println("Could not begin config stream\nREASON: " + configFirebaseData.errorReason());
  }
  Firebase.setStreamCallback(configFirebaseData, configStreamCallback, configStreamTimeoutCallback);
}

void addLogtoFirebase(float temperatureValue, unsigned long custom_timestamp) {
  Serial.println("------------------------------------");
  FirebaseJson log_json;
  //log_json.setDoubleDigits(3);
  log_json.add("temperature", temperatureValue);
  if (Firebase.pushJSON(loggingFirebaseData, "/essential_data", log_json)) {
    // Successfully pushed
    if (custom_timestamp == 0) {
      // Use Firebase's timestamp
      if (Firebase.setTimestamp(loggingFirebaseData, "/essential_data/" + loggingFirebaseData.pushName() + "/dateCreated")) {
        // Successfully set Firebase timestamp
        Serial.println("Added log to Firebase");
      }
      else {
        Serial.println("Failed to set Firebase timestamp: " + loggingFirebaseData.errorReason());
      }
    }
    else {
      // Set custom timestamp
      if (Firebase.setInt(loggingFirebaseData, "/essential_data/" + loggingFirebaseData.pushName() + "/dateCreated", custom_timestamp)) {
        // Successfully set custom timestamp
        Serial.println("Added CUSTOM log to Firebase");
      }
      else {
        Serial.println("Failed to set CUSTOM timestamp: " + loggingFirebaseData.errorReason());
      }
    }
  }
  else {
    Serial.println("Failed to log to Firebase: " + loggingFirebaseData.errorReason());
  }
  Serial.println("------------------------------------");
}

void configStreamCallback(StreamData data) {
  if (data.dataType() == "json") {
    FirebaseJson *config_json = data.to<FirebaseJson *>();
    FirebaseJsonData enableLoggingResult;
    FirebaseJsonData logIntervalResult;
    FirebaseJsonData temperatureLimitResult;
    config_json->get(enableLoggingResult, "enableLogging");
    config_json->get(logIntervalResult, "logInterval");
    config_json->get(temperatureLimitResult, "temperatureLimit");
    if (enableLoggingResult.success)
    {
      enableLogging = enableLoggingResult.to<bool>();
    }
    if (logIntervalResult.success)
    {
      logInterval = logIntervalResult.to<String>().toFloat() * 1000;
    }
    if (temperatureLimitResult.success)
    {
      temperatureLimit = temperatureLimitResult.to<String>().toFloat();
    }
    updateConfigEEPROM();
  }
}

void configStreamTimeoutCallback(bool timeout) {
  if (timeout) {
    Serial.println("\nConfig stream timeout, resume streaming...\n");
  }
  if (!configFirebaseData.httpConnected()) {
    Serial.printf("Config stream error code: %d, reason: %s\n\n", configFirebaseData.httpCode(), configFirebaseData.errorReason().c_str());
  }
}
