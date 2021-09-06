#include "firebase.h"
#include "../eeprom/eeprom.h"
#include "addons/TokenHelper.h" //Provide the token generation process info.
#include "addons/RTDBHelper.h"  //Provide the RTDB payload printing info and other helper functions.

FirebaseAuth auth;     // The user UID can be obtained from auth.token.uid
FirebaseConfig config; // FirebaseConfig data for config data
FirebaseData configReceiveFirebaseData;
FirebaseData configUpdateFirebaseData;
FirebaseData configLogUpdateFirebaseData;
FirebaseData loggingFirebaseData;

void firebaseSetup()
{
  Serial.printf("Initializing Firebase v%s\n\n", FIREBASE_CLIENT_VERSION);
  config.api_key = API_KEY;
  auth.user.email = USER_EMAIL;
  auth.user.password = USER_PASSWORD;
  config.database_url = DATABASE_URL;
  Firebase.reconnectWiFi(true);
  config.token_status_callback = tokenStatusCallback; //Assign the callback function for the long running token generation task, see addons/TokenHelper.h
  config.max_token_generation_retry = 5;              // Assign the maximum retry of token generation
  Firebase.begin(&config, &auth);                     // Initialize the library with the Firebase authen and config
  //configReceiveFirebaseData.setResponseSize(4096);
  if (!Firebase.beginStream(configReceiveFirebaseData, "/config"))
  {
    Serial.println("Could not begin config stream\nREASON: " + configReceiveFirebaseData.errorReason());
  }
  Firebase.setStreamCallback(configReceiveFirebaseData, configStreamCallback, configStreamTimeoutCallback);
}

void addLogtoFirebase(float temperatureValue, unsigned long custom_timestamp)
{
  Serial.println("------------------------------------");
  FirebaseJson log_json;
  //log_json.setDoubleDigits(3);
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
      // Set custom timestamp
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
  FirebaseJson config_json;
  config_json.add("enableLogging", enableLogging);
  config_json.add("logInterval", logInterval / 1000);
  config_json.add("temperatureLimit", temperatureLimit);
  if (Firebase.ready() && WiFi.status() == WL_CONNECTED && Firebase.setJSON(configUpdateFirebaseData, "/config", config_json))
  {
    Serial.println("Updated config in Firebase");
    config_json.add("uid", "device");
    if (Firebase.pushJSON(configLogUpdateFirebaseData, "/config_changes", config_json))
    {
      // Successfully pushed
      if (Firebase.setTimestamp(configLogUpdateFirebaseData, "/config_changes/" + configLogUpdateFirebaseData.pushName() + "/dateCreated"))
      {
        // Successfully set Firebase timestamp
        Serial.println("Added config change to Firebase");
      }
      else
      {
        Serial.println("Failed to set Firebase timestamp for config change: " + configLogUpdateFirebaseData.errorReason());
      }
    }
    else
    {
      Serial.println("Failed to update config change to Firebase: " + configLogUpdateFirebaseData.errorReason());
    }
    return true;
  }
  else
  {
    pendingConfigFirebaseUpdate = true;
    Serial.println("Failed to update config in Firebase: " + configUpdateFirebaseData.errorReason());
    return false;
  }
}

void configStreamCallback(StreamData data)
{
  if (data.dataType() == "json")
  {
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

void configStreamTimeoutCallback(bool timeout)
{
  if (timeout)
  {
    Serial.println("\nConfig stream timeout, resume streaming...\n");
  }
  if (!configReceiveFirebaseData.httpConnected())
  {
    Serial.printf("Config stream error code: %d, reason: %s\n\n", configReceiveFirebaseData.httpCode(), configReceiveFirebaseData.errorReason().c_str());
  }
}

/* bool updateEnableLoggingFirebase(bool value)
{
  if (Firebase.ready() && WiFi.status() == WL_CONNECTED && Firebase.setBoolAsync(loggingFirebaseData, "/config/enableLogging", value))
  {
    Serial.println("Updated enableLogging in Firebase");
    return true;
  }
  else
  {
    updateFirebaseConfig = true;
    Serial.println("Failed to update enableLogging in Firebase: " + loggingFirebaseData.errorReason());
    return false;
  }
} */
/* bool updateLogIntervalFirebase(float value)
{
  // In seconds
  if (Firebase.ready() && WiFi.status() == WL_CONNECTED && Firebase.setFloatAsync(loggingFirebaseData, "/config/logInterval", value))
  {
    Serial.println("Updated logInterval in Firebase");
    return true;
  }
  else
  {
    updateFirebaseConfig = true;
    Serial.println("Failed to update logInterval in Firebase: " + loggingFirebaseData.errorReason());
    return false;
  }
}
bool updateTemperatureLimitFirebase(float value)
{
  // In Celcius
  if (Firebase.ready() && WiFi.status() == WL_CONNECTED && Firebase.setFloatAsync(loggingFirebaseData, "/config/temperatureLimit", value))
  {
    Serial.println("Updated temperatureLimit in Firebase");
    return true;
  }
  else
  {
    updateFirebaseConfig = true;
    Serial.println("Failed to update temperatureLimit in Firebase: " + loggingFirebaseData.errorReason());
    return false;
  }
} */