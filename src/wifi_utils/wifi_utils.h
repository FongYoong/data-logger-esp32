#ifndef WIFI_UTILS_H
#define WIFI_UTILS_H

#include "Arduino.h"
#include <WiFi.h>
// WiFi credentials
#define WIFI_SSID "andrewc@unifi"
#define WIFI_PASSWORD "68488765"
#define WIFI_RECONNECT_INTERVAL 10000 // Milliseconds // Interval between reconnecting Wi-Fi

void wifiSetup();
void WiFiStationConnected(WiFiEvent_t event, WiFiEventInfo_t info);
void WiFiGotIP(WiFiEvent_t event, WiFiEventInfo_t info);
void WiFiStationDisconnected(WiFiEvent_t event, WiFiEventInfo_t info);

#endif