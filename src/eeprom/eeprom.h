#ifndef EEPROM_H
#define EEPROM_H

#include "Arduino.h"
#include <Preferences.h>
extern Preferences preferences; // Preferences object to access flash memory

#define MIN_OFFLINE_LOGS_OFFSET 0 // Minimum index of offline logs
#define MAX_OFFLINE_LOGS 10 // Maximum logs stored in EEPROM
#define ADD_OFFLINE_LOG_INTERVAL 4000 // Milliseconds // Interval between writes to EEPROM
#define UPLOAD_OFFLINE_LOGS_INTERVAL 10000 // Milliseconds // Interval between checking and uploading EEPROM logs

// Access global variables
extern bool demoMode;
extern bool enableLogging;
extern unsigned long logInterval;
extern float temperatureLimit;

void EEPROMSetup();
void updateConfigEEPROM();
void addOfflineLog(float temperature);
void uploadOfflineLogs();

#endif