#include "unix_time.h"

const long gmtOffset_sec = 28800; // 8 hours * 3600 seconds
const int daylightOffset_sec = 3600;
const char* ntpServer = "pool.ntp.org"; // NTP server to request epoch time

void timeSetup() {
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
}

unsigned long getUnixTime() {
  time_t now;
  struct tm timeinfo;
  if (!getLocalTime(&timeinfo)) {
    Serial.println("Failed to obtain time");
    return (0);
  }
  time(&now);
  return now;
}
