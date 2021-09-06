#include "rtos.h"
#include "../firebase/firebase.h"
#include "../eeprom/eeprom.h"
#include "../user_interface/user_interface.h"

TaskHandle_t logTaskHandler;
TaskHandle_t userInterfaceTaskHandler;
TaskHandle_t uploadOfflineLogsTaskHandler;
TaskHandle_t wifiReconnectTaskHandler;

void rtosSetup() {
    xTaskCreatePinnedToCore(
        LogTask, /* Task function. */
        "LogTask",   /* name of task. */
        1024,     /* Stack size of task */
        NULL,      /* parameter of the task */
        LOG_PRIORITY,         /* priority of the task */
        &logTaskHandler,    /* Task handle to keep track of created task */
        tskNO_AFFINITY
    );  // Use any available core
    xTaskCreatePinnedToCore(
        UserInterfaceTask, /* Task function. */
        "UserInterfaceTask",   /* name of task. */
        1024,     /* Stack size of task */
        NULL,      /* parameter of the task */
        USER_INTERFACE_PRIORITY,         /* priority of the task */
        &userInterfaceTaskHandler,    /* Task handle to keep track of created task */
        tskNO_AFFINITY
    );  // Use any available core
    xTaskCreatePinnedToCore(
        UploadOfflineLogsTask, /* Task function. */
        "UploadOfflineLogsTask",   /* name of task. */
        1024,     /* Stack size of task */
        NULL,      /* parameter of the task */
        UPLOAD_OFFLINE_LOGS_PRIORITY,         /* priority of the task */
        &uploadOfflineLogsTaskHandler,    /* Task handle to keep track of created task */
        tskNO_AFFINITY
    );  // Use any available core
    xTaskCreatePinnedToCore(
        WifiReconnectTask, /* Task function. */
        "wifiReconnectTask",   /* name of task. */
        1024,     /* Stack size of task */
        NULL,      /* parameter of the task */
        WIFI_RECONNECT_PRIORITY,         /* priority of the task */
        &wifiReconnectTaskHandler,    /* Task handle to keep track of created task */
        tskNO_AFFINITY
    );  // Use any available core
}

void LogTask(void *parameter){
  for(;;){
    vTaskDelay(500 / portTICK_PERIOD_MS); // Pause the task for 500ms

    vTaskDelay(500 / portTICK_PERIOD_MS); // Pause the task for 500ms
  }
}

void UserInterfaceTask(void *parameter){
  for(;;){
    vTaskDelay(500 / portTICK_PERIOD_MS); // Pause the task for 500ms

    vTaskDelay(500 / portTICK_PERIOD_MS); // Pause the task for 500ms
  }
}

void UploadOfflineLogsTask(void *parameter){
  for(;;){
    vTaskDelay(500 / portTICK_PERIOD_MS); // Pause the task for 500ms

    vTaskDelay(500 / portTICK_PERIOD_MS); // Pause the task for 500ms
  }
}

void WifiReconnectTask(void *parameter){
  for(;;){
    vTaskDelay(500 / portTICK_PERIOD_MS); // Pause the task for 500ms

    vTaskDelay(500 / portTICK_PERIOD_MS); // Pause the task for 500ms
  }
}