/* WiFi station Example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/
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


// from wifi.c
void wifiTask(void * pvParameters);


static const char *TAG = "main";
TaskHandle_t wifiTaskHandle = NULL;


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
    ESP_ERROR_CHECK(nvs_flash_init());

    // run wifi task at priority 1
    xTaskCreate(wifiTask, "wifi", 4048, NULL, 1, &wifiTaskHandle);
}
