#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <math.h>
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
#include "cJSON.h"
#include "pump.h"

static const char *TAG = "thingsboard";


// event group showing if we are connected to thingsboard
EventGroupHandle_t mqtt_event_group;
#define MQTT_CONNECTED_BIT BIT0


// from mqtt.c
void mqtt_app_start(void);
extern esp_mqtt_client_handle_t mqtt_client;


void handle_request(const char * method, const char * response_topic)
{
    ESP_LOGI(TAG, "method: %s, response: %s", method, response_topic);
    if(!strcmp(method, "getValue"))
    {
        // %f format is not included to keep the sdk small
        double ms = getPumpingTimeMs();
        int before = (int)floor(ms / 1000);
        int after = (int)round(ms / 10) % 100;

        char number[10];        
        int len = snprintf(number,  10,  "%d.%02d", before, after);
        esp_mqtt_client_publish(mqtt_client, response_topic, number, len, 1, 0);
    }
    else if (!strcmp(method, "waterNow"))
    {

    }
}

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
   

    // periodically send telemetry data
    TickType_t xLastWakeTime = xTaskGetTickCount();
    while(true)
    {

        cJSON *telemetry = cJSON_CreateObject();

        if (cJSON_AddNumberToObject(telemetry, "Humidity", 21) == NULL)
        {
            goto end;
        }
        if (cJSON_AddNumberToObject(telemetry, "Temperature", 37) == NULL)
        {
            goto end;
        }


        char * string = cJSON_Print(telemetry);
        if (string != NULL)
        {
            ESP_LOGI(TAG, "Publishing telemetry: %s", string);
            esp_mqtt_client_publish(mqtt_client, TELEMETRY_TOPIC, string, 0, 1, 0);
        }
    
    end:
        cJSON_Delete(telemetry);

        // execute every 10s
        vTaskDelayUntil( &xLastWakeTime, 10000 / portTICK_PERIOD_MS );
    }
}