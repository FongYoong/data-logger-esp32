#ifndef WIFI_UTILS_H
#define WIFI_UTILS_H

#include "Arduino.h"
#include <WiFi.h>
// WiFi credentials
#define WIFI_SSID "andrewc@unifi"
#define WIFI_PASSWORD "68488765"
#define MAX_WIFI_RETRIES 10
#define WIFI_RETRY_INTERVAL 60000

void wifiSetup();
void WiFiStationConnected(WiFiEvent_t event, WiFiEventInfo_t info);
void WiFiGotIP(WiFiEvent_t event, WiFiEventInfo_t info);
void WiFiStationDisconnected(WiFiEvent_t event, WiFiEventInfo_t info);

#endif