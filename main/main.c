#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_system.h"
#include "esp_log.h"
#include "esp_netif.h"
#include "esp_event.h"
#include "esp_wifi.h"
#include "nvs.h"
#include "nvs_flash.h"

#include "lwip/err.h"
#include "lwip/sys.h"

#include "pump.h"
#include "wifi.h"
#include "thingsboard.h"
#include "nvs.h"



// from wifi.c
TaskHandle_t wifiTaskHandle = NULL;

// from pump.c
TaskHandle_t waterPumpTaskHandle = NULL;

// from thingsboard.c
TaskHandle_t thingsboardTaskHandle = NULL;


static const char *TAG = "main";


void debug_tasks()
{
    char buffer[2048];
    vTaskList(buffer);
    ESP_LOGI(TAG,
    "\n***********************************************\n"
    "Task           	State   Prio    Stack    Num\n"
    "***********************************************\n"
    "%s\n"
    "***********************************************\n",
    buffer);
}

// this task runs at priority 1
// its a task that runs sytem setup then deletes itself
void app_main()
{
    initPump();

    initNVS();

    // run pump task at priority 14
    xTaskCreate(waterPumpTask, "pump", 2048, NULL, 14, &waterPumpTaskHandle);

    // run wifi task
    xTaskCreate(wifiTask, "wifi", 2048, NULL, 2, &wifiTaskHandle);

    // run thingsboard task
    xTaskCreate(thingsboardTask, "tb", 7000, NULL, 1, &thingsboardTaskHandle);
}
