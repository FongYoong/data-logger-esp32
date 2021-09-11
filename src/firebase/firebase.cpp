#include "firebase.h"
#include "../eeprom/eeprom.h"
#include "addons/TokenHelper.h" //Provide the token generation process info.
#include "addons/RTDBHelper.h"  //Provide the RTDB payload printing info and other helper functions.

FirebaseAuth auth;     // The user UID can be obtained from auth.token.uid
FirebaseConfig config; // Firebase Config data
FirebaseData configReceiveFirebaseData; // Firebase Data received from config changes
FirebaseData loggingFirebaseData; // Firebase Data received from uploading logs

void firebaseSetup()
{
  Serial.printf("Initializing Firebase v%s\n\n", FIREBASE_CLIENT_VERSION);
  // Firebase Credentials
  config.api_key = API_KEY;
  auth.user.email = USER_EMAIL;
  auth.user.password = USER_PASSWORD;
  config.database_url = DATABASE_URL;
  Firebase.reconnectWiFi(true); // Reconnect when possible
  config.token_status_callback = tokenStatusCallback; // Token generation for authentication
  config.max_token_generation_retry = 5;              // Maximum retries for token generation
  Firebase.begin(&config, &auth);                     // Initialize Firebase authentication and config
  if (!Firebase.beginStream(configReceiveFirebaseData, "/config"))
  {
    Serial.println("Could not begin config stream\nREASON: " + configReceiveFirebaseData.errorReason());
  }
  // Create a stream to watch for changes in the '/config' location
  Firebase.setStreamCallback(configReceiveFirebaseData, configStreamCallback, configStreamTimeoutCallback);
}

void addLogtoFirebase(float temperatureValue, unsigned long custom_timestamp)
{
  // Upload log to Firebase
  Serial.println("------------------------------------");
  FirebaseJson log_json; // JSON object to send to Firebase
  log_json.add("temperature", temperatureValue);
  if (Firebase.pushJSON(loggingFirebaseData, "/essential_data", log_json))
  {
    // Successfully pushed
    if (custom_timestamp == 0)
    {
      // Use Firebase's timestamp
      if (Firebase.setTimestamp(loggingFirebaseData, "/essential_data/" + loggingFirebaseData.pushName() + "/dateCreated"))
      {
        // Successfully set Firebase timestamp
        Serial.println("Added log to Firebase");
      }
      else
      {
        Serial.println("Failed to set Firebase timestamp: " + loggingFirebaseData.errorReason());
      }
    }
    else
    {
      // Use custom timestamp
      if (Firebase.setInt(loggingFirebaseData, "/essential_data/" + loggingFirebaseData.pushName() + "/dateCreated", custom_timestamp))
      {
        // Successfully set custom timestamp
        Serial.println("Added CUSTOM log to Firebase");
      }
      else
      {
        Serial.println("Failed to set CUSTOM timestamp in Firebase: " + loggingFirebaseData.errorReason());
      }
    }
  }
  else
  {
    Serial.println("Failed to log to Firebase: " + loggingFirebaseData.errorReason());
  }
  Serial.println("------------------------------------");
}

bool updateConfigFirebase()
{
  // Update latest config in Firebase
  FirebaseJson config_json; // JSON object to send to Firebase
  config_json.add("demoMode", demoMode);
  config_json.add("enableLogging", enableLogging);
  config_json.add("logInterval", logInterval);
  config_json.add("temperatureLimit", temperatureLimit);
  if (Firebase.ready() && WiFi.status() == WL_CONNECTED && Firebase.setJSON(loggingFirebaseData, "/config", config_json))
  {
    // Successfully updated config in Firebase
    Serial.println("Updated config in Firebase");
    FirebaseJson change_json;
    change_json.add("demoMode", demoMode);
    change_json.add("enableLogging", enableLogging);
    change_json.add("logInterval", logInterval);
    change_json.add("temperatureLimit", temperatureLimit);
    change_json.add("uid", "device");
    // Record config change in Firebase
    if (Firebase.pushJSON(loggingFirebaseData, "/config_changes", change_json))
    {
      // Successfully pushed
      if (Firebase.setTimestamp(loggingFirebaseData, "/config_changes/" + loggingFirebaseData.pushName() + "/dateCreated"))
      {
        // Successfully set Firebase timestamp
        Serial.println("Added config CHANGE to Firebase");
      }
      else
      {
        Serial.println("Failed to set Firebase timestamp for config CHANGE: " + loggingFirebaseData.errorReason());
      }
    }
    else
    {
      Serial.println("Failed to update config CHANGE to Firebase: " + loggingFirebaseData.errorReason());
    }
    return true;
  }
  else
  {
    // Failed to update config in Firebase
    pendingConfigFirebaseUpdate = true;
    Serial.println("Failed to update config in Firebase: " + loggingFirebaseData.errorReason());
    return false;
  }
}

void configStreamCallback(StreamData data)
{
  // Triggered when a config change occurs in Firebase
  if (data.dataType() == "json")
  {
    FirebaseJson *config_json = data.to<FirebaseJson *>(); // Config data received from Firebase
    FirebaseJsonData demoModeResult;
    FirebaseJsonData enableLoggingResult;
    FirebaseJsonData logIntervalResult;
    FirebaseJsonData temperatureLimitResult;
    config_json->get(demoModeResult, "demoMode");
    config_json->get(enableLoggingResult, "enableLogging");
    config_json->get(logIntervalResult, "logInterval");
    config_json->get(temperatureLimitResult, "temperatureLimit");
    if (demoModeResult.success)
    {
      demoMode = demoModeResult.to<bool>();
    }
    if (enableLoggingResult.success)
    {
      enableLogging = enableLoggingResult.to<bool>();
    }
    if (logIntervalResult.success)
    {
      logInterval = atol(logIntervalResult.to<String>().c_str());
    }
    if (temperatureLimitResult.success)
    {
      temperatureLimit = temperatureLimitResult.to<String>().toFloat();
    }
    updateConfigEEPROM(); // Update EEPROM values
  }
}

void configStreamTimeoutCallback(bool timeout)
{
  // Triggered if config stream is unresponsive or loses connection
  if (timeout)
  {
    Serial.println("\nConfig stream timeout, resume streaming...\n");
  }
  if (!configReceiveFirebaseData.httpConnected())
  {
    Serial.printf("Config stream error code: %d, reason: %s\n\n", configReceiveFirebaseData.httpCode(), configReceiveFirebaseData.errorReason().c_str());
  }
}