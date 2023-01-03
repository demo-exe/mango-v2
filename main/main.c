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





static const char *TAG = "main";

static int s_retry_num = 0;

void debug_tasks()
{
    char buffer[2048];
    vTaskList(buffer);
    ESP_LOGI(TAG,
    "\n**********************************\n"
    "Task           	State   Prio    Stack    Num\n"
    "**********************************\n"
    "%s\n"
    "**********************************",
    buffer);
}


void app_main()
{
    debug_tasks();
    ESP_ERROR_CHECK(nvs_flash_init());

    debug_tasks();
    ESP_LOGI(TAG, "ESP_WIFI_MODE_STA");
    wifi_init_sta();
}
