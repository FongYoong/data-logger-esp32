#ifndef EEPROM_H
#define EEPROM_H

#include "Arduino.h"
#include <Preferences.h>
extern Preferences preferences;

#define MAX_OFFLINE_LOGS 10
#define ADD_OFFLINE_LOG_INTERVAL 4000 // Milliseconds // Interval between writes to EEPROM
#define UPLOAD_OFFLINE_LOGS_INTERVAL 10000 // Milliseconds // Interval between checking and uploading EEPROM logs

extern bool enableLogging;
extern float logInterval;
extern float temperatureLimit;

void EEPROMSetup();
void updateConfigEEPROM();
void addOfflineLog(float temperature);
void uploadOfflineLogs();

#endif