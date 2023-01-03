#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include "esp_wifi.h"
#include "esp_system.h"
#include "nvs_flash.h"
#include "esp_event.h"
#include "esp_netif.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "freertos/semphr.h"
#include "freertos/queue.h"
#include "esp_log.h"
#include "mqtt_client.h"

#include "wifi.h"
#include "thingsboard.h"

static const char *TAG = "thingsboard";


// event group showing if we are connected to thingsboard
EventGroupHandle_t mqtt_event_group;
#define MQTT_CONNECTED_BIT BIT0

esp_mqtt_client_handle_t client;

// from mqtt.c
void mqtt_app_start(void);


void thingsboardTask(void * pvParameters)
{
    mqtt_event_group = xEventGroupCreate();

    // wait until wifi first connected
    while(true)
    {
        EventBits_t bits = xEventGroupWaitBits(s_wifi_event_group, WIFI_CONNECTED_BIT, pdFALSE, pdFALSE, portMAX_DELAY);
        if (bits & WIFI_CONNECTED_BIT)
        {
            mqtt_app_start();
            break;
        }
    }
    vTaskDelete(NULL);

            // msg_id = esp_mqtt_client_publish(client, "/topic/qos1", "data_3", 0, 1, 0);
            // ESP_LOGI(TAG, "sent publish successful, msg_id=%d", msg_id);
}