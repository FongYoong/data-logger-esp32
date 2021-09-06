#define MDASH_APP_NAME "DataLogger"
#define MDASH_DEVICE_PASSWORD "fJk90L99Sy91G5WgYDO4Kg7qQ"
#include <mDash.h>

bool enableLogging = true;
float logInterval = 1000; // Milliseconds
float temperatureLimit = 25; // Celcius

// Time tracking
unsigned long wifiPrevMillis = 0;
unsigned long configPrevMillis = 0;
unsigned long loggingPrevMillis = 0;
unsigned long addOfflineLogsPrevMillis = 0;
unsigned long uploadOfflineLogsPrevMillis = 0;
unsigned long buttonPrevMillis = 0;

#include "src/wifi_utils/wifi_utils.h"
#include "src/eeprom/eeprom.h"
#include "src/firebase/firebase.h"
#include "src/unix_time/unix_time.h"
#include "src/pins/pins.h"
#include "src/user_interface/user_interface.h"
#include "src/rtos/rtos.h"

void setup() {
  Serial.begin(115200);
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

  // If offline, try reconnecting
  if ((WiFi.status() != WL_CONNECTED) && (millis() - wifiPrevMillis > WIFI_RETRY_INTERVAL || wifiPrevMillis == 0)) {
    Serial.println("Reconnecting to WiFi...");
    WiFi.disconnect();
    WiFi.reconnect();
    wifiPrevMillis = millis();
  }

  // Print config info
//  if (millis() - configPrevMillis > 2000 || configPrevMillis == 0) {
//    Serial.println(enableLogging ? "Enabled" : "Disabled");
//    Serial.println("Log Interval: " + String(logInterval));
//    Serial.println("Temperature Limit: " + String(temperatureLimit));
//    configPrevMillis = millis();
//  }

  // Log data
  if (enableLogging && (millis() - loggingPrevMillis > logInterval || loggingPrevMillis == 0)) {
    int potmValue = analogRead(potmPin);    // read the value from the sensor
    float temperatureValue = map(potmValue, 0, 4095, 0, 50);
    Serial.println("------------------------------------");
    Serial.println("PotmValue: " + String(potmValue));
    Serial.println("Temperature: " + String(temperatureValue));
    if (temperatureValue > temperatureLimit) {
      // Exceed limit
      Serial.println("Exceeded Temperature limit!!!");
      digitalWrite(warning_LED, HIGH);
    }
    else {
      // Within limit, Do something like blink LED
      digitalWrite(warning_LED, LOW);
    }
    if (Firebase.ready() && WiFi.status() == WL_CONNECTED) {
      addLogtoFirebase(temperatureValue);
    }
    else {
      // Add log to EEPROM if offline
      if (millis() - addOfflineLogsPrevMillis > ADD_OFFLINE_LOG_INTERVAL || addOfflineLogsPrevMillis == 0) {
        addOfflineLog(temperatureValue);
        addOfflineLogsPrevMillis = millis();
      }
    }
    loggingPrevMillis = millis();
    Serial.println("------------------------------------");
  }

  // Upload leftover offline logs
  if (Firebase.ready() && WiFi.status() == WL_CONNECTED && (millis() - uploadOfflineLogsPrevMillis > UPLOAD_OFFLINE_LOGS_INTERVAL || uploadOfflineLogsPrevMillis == 0)) {
    uploadOfflineLogs();
    uploadOfflineLogsPrevMillis = millis();
  }

//  int pressEnter = digitalRead(button_enter);
//
//  if ((millis() - buttonPrevMillis > DEBOUNCE_DELAY || buttonPrevMillis == 0) && pressEnter == LOW)
//  {
//    WiFi.disconnect();
//    Serial.println("Disconnected from WIFI");
//    buttonPrevMillis = millis();
//  }

}
