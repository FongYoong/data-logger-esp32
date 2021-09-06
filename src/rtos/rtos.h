#ifndef RTOS_H
#define RTOS_H

#include "Arduino.h"

#define LOG_PRIORITY 4
#define USER_INTERFACE_PRIORITY 3
#define UPLOAD_OFFLINE_LOGS_PRIORITY 2
#define WIFI_RECONNECT_PRIORITY 1

extern bool enableLogging;
extern float logInterval;
extern float temperatureLimit;

void rtosSetup();
void LogTask(void *parameter);
void UserInterfaceTask(void *parameter);
void UploadOfflineLogsTask(void *parameter);
void WifiReconnectTask(void *parameter);

#endif