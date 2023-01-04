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

#include <dht.h>

#include "wifi.h"
#include "thingsboard.h"
#include "cJSON.h"
#include "pump.h"
#include "nvs.h"

static const char *TAG = "thingsboard";


// event group showing if we are connected to thingsboard
EventGroupHandle_t mqtt_event_group;
#define MQTT_CONNECTED_BIT BIT0


// from mqtt.c
void mqtt_app_start(void);
extern esp_mqtt_client_handle_t mqtt_client;


void handle_request(const char * data, const char * response_topic)
{
    cJSON *json = cJSON_Parse(data);
    cJSON *method = cJSON_GetObjectItemCaseSensitive(json, "method");
    if (!(cJSON_IsString(method) && (method->valuestring != NULL)))
        return;

    if(!strcmp(method->valuestring, "getValue"))
    {
        char number[10];        
        int len = snprintf(number,  10,  "%.2f", getPumpingTimeMs() / 1000.);
        esp_mqtt_client_publish(mqtt_client, response_topic, number, len, 1, 0);
    }
    else if (!strcmp(method->valuestring, "setValue"))
    {
        cJSON *value = cJSON_GetObjectItemCaseSensitive(json, "params");
        if(cJSON_IsNumber(value))
        {
            setPumpingTimeMs(value->valuedouble* 1000);
        }
    }
    else if (!strcmp(method->valuestring, "waterNow"))
    {
        waterNow();
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
    vTaskDelay(pdMS_TO_TICKS(2000));
    
    // periodically send telemetry data
    TickType_t xLastWakeTime = xTaskGetTickCount();
    while(true)
    {
        float temperature, humidity;
        gpio_set_pull_mode(2, GPIO_PULLUP_ONLY);

        if (dht_read_float_data(DHT_TYPE_AM2301, 2, &humidity, &temperature) == ESP_OK)
            printf("Humidity: %.1f%% Temp: %.1fC\n", humidity, temperature);
        else
            printf("Could not read data from sensor\n");

        // If you read the sensor data too often, it will heat up
        // http://www.kandrsmith.org/RJS/Misc/Hygrometers/dht_sht_how_fast.html
        vTaskDelay(pdMS_TO_TICKS(2000));

        cJSON *telemetry = cJSON_CreateObject();

        if (cJSON_AddNumberToObject(telemetry, "Humidity", humidity) == NULL)
        {
            goto end;
        }
        if (cJSON_AddNumberToObject(telemetry, "Temperature", temperature) == NULL)
        {
            goto end;
        }


        char * string = cJSON_PrintUnformatted(telemetry);
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
    
    vTaskSuspend(NULL);
}