#include "wifi_utils.h"

unsigned char wifiRetries = 0;

void wifiSetup() {
  WiFi.disconnect(true);  // delete old config
  WiFi.onEvent(WiFiStationConnected, SYSTEM_EVENT_STA_CONNECTED);
  WiFi.onEvent(WiFiGotIP, SYSTEM_EVENT_STA_GOT_IP);
  WiFi.onEvent(WiFiStationDisconnected, SYSTEM_EVENT_STA_DISCONNECTED);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to Wi-Fi");
  while (wifiRetries < 10 && WiFi.status() != WL_CONNECTED)
  {
    // Attempt to reconnect
    Serial.print(".");
    delay(300);
    wifiRetries++;
  }
  if (WiFi.status() == WL_CONNECTED) {
//    Serial.println("Connected with IP: ");
//    Serial.print(WiFi.localIP());
//    Serial.println();
  }
  else {
    Serial.println("Operating in Offline Mode");
  }
}

void WiFiStationConnected(WiFiEvent_t event, WiFiEventInfo_t info){
  Serial.println("Connected to AP successfully!");
}

void WiFiGotIP(WiFiEvent_t event, WiFiEventInfo_t info){
    Serial.println("Connected with IP: ");
    Serial.print(WiFi.localIP());
    Serial.println();
}

void WiFiStationDisconnected(WiFiEvent_t event, WiFiEventInfo_t info){
  Serial.println("Disconnected from WiFi access point");
  Serial.print("WiFi lost connection. Reason: ");
  Serial.println(info.disconnected.reason);
}