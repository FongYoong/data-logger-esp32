#define MDASH_APP_NAME "DataLogger"
#define MDASH_DEVICE_PASSWORD "fJk90L99Sy91G5WgYDO4Kg7qQ"
#include <mDash.h>

////////////////////////////////////////////////////////
// Global variables
bool demoMode = true; // If true, read temp. value from potentiometer. Otherwise, read from DHT11
bool pendingConfigFirebaseUpdate = false; // If true, update config values in Firebase
bool enableLogging = true; //  Log if true
unsigned long logInterval = 1000; // Interval between logs // Milliseconds
float temperatureLimit = 25; // Warn if exceed this limit // Celcius
float temperatureValue = 0; // Temeperature given by DHT11 or potentiometer //  Celcius
////////////////////////////////////////////////////////

#include "src/wifi_utils/wifi_utils.h"
#include "src/eeprom/eeprom.h"
#include "src/firebase/firebase.h"
#include "src/unix_time/unix_time.h"
#include "src/pins/pins.h"
#include "src/user_interface/user_interface.h"

////////////////////////////////////////////////////////
// Time tracking
unsigned long wifiPrevMillis = 0;
unsigned long configPrevMillis = 0;
unsigned long loggingPrevMillis = 0;
unsigned long addOfflineLogsPrevMillis = 0;
unsigned long uploadOfflineLogsPrevMillis = 0;
unsigned long pendingConfigFirebaseUpdatePrevMillis = 0;
unsigned long buttonPrevMillis = 0;
////////////////////////////////////////////////////////

void setup()
{
  Serial.begin(115200);
  pinsSetup();
  EEPROMSetup();
  wifiSetup();
  mDashBegin(MDASH_DEVICE_PASSWORD);
  firebaseSetup();
  timeSetup();
  userInterfaceSetup();
}

void loop()
{
  if (demoMode) {
    int potmValue = analogRead(potmPin); // Read potentiometer analog value
    temperatureValue = float(potmValue) * 50.0 / 4095.0; //12-bit ADC = 4095 = 50 Celcius
  }
  else {
    float temp = dht.readTemperature(); // Read from DHT11
    // Check if failed
    if (isnan(temp) ) {
      Serial.println(F("Failed to read from DHT sensor!"));
    }
    else {
      temperatureValue = temp; // Set if successful
    }
  }

  if (temperatureValue > temperatureLimit)
  {
    // Exceed limit, turn on warning LED
    Serial.println("Exceeded Temperature limit!!!");
    digitalWrite(warning_LED, HIGH);
  }
  else
  {
    digitalWrite(warning_LED, LOW); // Within limit, turn off warning LED
  }

  // If offline, try reconnecting
  if ((WiFi.status() != WL_CONNECTED) && (millis() - wifiPrevMillis > WIFI_RECONNECT_INTERVAL || wifiPrevMillis == 0))
  {
    Serial.println("Reconnecting to WiFi...");
    WiFi.disconnect();
    WiFi.reconnect();
    wifiPrevMillis = millis(); // Reset time
  }

  // Log data
  if (enableLogging && (millis() - loggingPrevMillis > logInterval || loggingPrevMillis == 0))
  {
    Serial.println("------------------------------------");
    Serial.println("Temperature: " + String(temperatureValue));
    if (Firebase.ready() && WiFi.status() == WL_CONNECTED)
    {
      addLogtoFirebase(temperatureValue); // Upload log if online
    }
    else
    {
      // Add log to EEPROM if offline
      const unsigned long interval = logInterval > ADD_OFFLINE_LOG_INTERVAL ? logInterval : ADD_OFFLINE_LOG_INTERVAL; // Use the longest interval
      if (millis() - addOfflineLogsPrevMillis > interval || addOfflineLogsPrevMillis == 0)
      {
        addOfflineLog(temperatureValue); // Store offline log in EEPROM
        addOfflineLogsPrevMillis = millis(); // Reset time
      }
    }
    loggingPrevMillis = millis(); // Reset time
    Serial.println("------------------------------------");
  }

  // Upload leftover offline logs
  if (Firebase.ready() && WiFi.status() == WL_CONNECTED && (millis() - uploadOfflineLogsPrevMillis > UPLOAD_OFFLINE_LOGS_INTERVAL || uploadOfflineLogsPrevMillis == 0))
  {
    uploadOfflineLogs(); // Upload pending offline logs to Firebase
    uploadOfflineLogsPrevMillis = millis(); // Reset time
  }

  // User Interface
  if (millis() - buttonPrevMillis > DEBOUNCE_DELAY || buttonPrevMillis == 0)
  {
    processInputs(); // Check for button presses
    buttonPrevMillis = millis(); // Reset time
  }
  renderUI(); // Display user interface

  // Update config in Firebase if required
  if (pendingConfigFirebaseUpdate && (millis() - pendingConfigFirebaseUpdatePrevMillis > UPDATE_FIREBASE_CONFIG_INTERVAL || pendingConfigFirebaseUpdatePrevMillis == 0))
  {
    pendingConfigFirebaseUpdate = !updateConfigFirebase(); // If Firebase update failed, set pendingConfigFirebaseUpdate to true
    pendingConfigFirebaseUpdatePrevMillis = millis(); // Reset time
  }

}
