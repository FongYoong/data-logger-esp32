#include "rtos.h"
#include <WiFi.h>
#include "../pins/pins.h"
#include "../wifi_utils/wifi_utils.h"
#include "../firebase/firebase.h"
#include "../eeprom/eeprom.h"
#include "../user_interface/user_interface.h"

TaskHandle_t logTaskHandler;
TaskHandle_t userInterfaceTaskHandler;
TaskHandle_t uploadOfflineLogsTaskHandler;
TaskHandle_t wifiReconnectTaskHandler;

unsigned long addOfflineLogsPrevTicks = 0;

void rtosSetup()
{
    xTaskCreate(
        LogTask,         /* Task function. */
        "LogTask",       /* name of task. */
        10000,            /* Stack size of task */
        NULL,            /* parameter of the task */
        LOG_PRIORITY,    /* priority of the task */
        &logTaskHandler /* Task handle to keep track of created task */
    );
    xTaskCreate(
        UserInterfaceTask,         /* Task function. */
        "UserInterfaceTask",       /* name of task. */
        1000,                      /* Stack size of task */
        NULL,                      /* parameter of the task */
        USER_INTERFACE_PRIORITY,   /* priority of the task */
        &userInterfaceTaskHandler /* Task handle to keep track of created task */
    );
    xTaskCreate(
        UploadOfflineLogsTask,         /* Task function. */
        "UploadOfflineLogsTask",       /* name of task. */
        10000,                          /* Stack size of task */
        NULL,                          /* parameter of the task */
        UPLOAD_OFFLINE_LOGS_PRIORITY,  /* priority of the task */
        &uploadOfflineLogsTaskHandler /* Task handle to keep track of created task */
        );               // Use any available core
    xTaskCreate(
        WifiReconnectTask,         /* Task function. */
        "wifiReconnectTask",       /* name of task. */
        512,                      /* Stack size of task */
        NULL,                      /* parameter of the task */
        WIFI_RECONNECT_PRIORITY,   /* priority of the task */
        &wifiReconnectTaskHandler /* Task handle to keep track of created task */
        );
}
// CONFIG_ARDUINO_RUNNING_CORE
// tskNO_AFFINITY

void LogTask(void *parameter)
{
    for (;;)
    {
        if (enableLogging)
        {
            int potmValue = analogRead(potmPin); // read the value from the sensor
            float temperatureValue = map(potmValue, 0, 4095, 0, 50);
            Serial.println("------------------------------------");
            Serial.println("PotmValue: " + String(potmValue));
            Serial.println("Temperature: " + String(temperatureValue));
            if (temperatureValue > temperatureLimit)
            {
                // Exceed limit
                Serial.println("Exceeded Temperature limit!!!");
                digitalWrite(warning_LED, HIGH);
            }
            else
            {
                // Within limit, Do something like blink LED
                digitalWrite(warning_LED, LOW);
            }
            if (Firebase.ready() && WiFi.status() == WL_CONNECTED)
            {
                addLogtoFirebase(temperatureValue);
            }
            else
            {
                // Add log to EEPROM if offline
                if (xTaskGetTickCount() - addOfflineLogsPrevTicks > (ADD_OFFLINE_LOG_INTERVAL / portTICK_PERIOD_MS) || addOfflineLogsPrevTicks == 0)
                {
                    addOfflineLog(temperatureValue);
                    addOfflineLogsPrevTicks = xTaskGetTickCount();
                }
            }
            Serial.println("------------------------------------");
            vTaskDelay(logInterval / portTICK_PERIOD_MS); // Pause the task
        }
        else {
            vTaskDelay(1000 / portTICK_PERIOD_MS); // Give time slice
        }
    }
}

void UserInterfaceTask(void *parameter)
{
    for (;;)
    {
        //UILoop();
        vTaskDelay(50000 / portTICK_PERIOD_MS); // Pause the task
    }
}

void UploadOfflineLogsTask(void *parameter)
{
    for (;;)
    {
        // Upload leftover offline logs
        if (Firebase.ready() && WiFi.status() == WL_CONNECTED)
        {
            uploadOfflineLogs();
            vTaskDelay(UPLOAD_OFFLINE_LOGS_INTERVAL / portTICK_PERIOD_MS); // Pause the task
        }
        else {
            vTaskDelay(100 / portTICK_PERIOD_MS); // Give time slice
        }
    }
}

void WifiReconnectTask(void *parameter)
{
    for (;;)
    {
        // If offline, try reconnecting
        if ((WiFi.status() != WL_CONNECTED))
        {
            Serial.println("Reconnecting to WiFi...");
            WiFi.disconnect();
            WiFi.reconnect();
        }
        vTaskDelay(WIFI_RECONNECT_INTERVAL / portTICK_PERIOD_MS); // Pause the task
    }
}