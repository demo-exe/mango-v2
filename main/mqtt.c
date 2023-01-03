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
#include "cJSON.h"

static const char *TAG = "mqtt";


// event group showing if we are connected to thingsboard
EventGroupHandle_t mqtt_event_group;
#define MQTT_CONNECTED_BIT BIT0

// handle to mqtt client
esp_mqtt_client_handle_t mqtt_client;


static void mqtt_event_handler(void *handler_args, esp_event_base_t base, int32_t event_id, void *event_data) {
    esp_mqtt_event_handle_t event = (esp_mqtt_event_handle_t) event_data;
    esp_mqtt_client_handle_t client = event->client;

    // your_context_t *context = event->context;
    switch (event->event_id) {
        case MQTT_EVENT_CONNECTED:
            ESP_LOGI(TAG, "MQTT_EVENT_CONNECTED");

            xEventGroupSetBits(mqtt_event_group, MQTT_CONNECTED_BIT);
            // subscribe to rpc calls
            esp_mqtt_client_subscribe(client, RPC_REQ_TOPIC, 0);

            break;
        case MQTT_EVENT_DISCONNECTED:
            ESP_LOGI(TAG, "MQTT_EVENT_DISCONNECTED");

            xEventGroupClearBits(mqtt_event_group, MQTT_CONNECTED_BIT);

            break;
        case MQTT_EVENT_SUBSCRIBED:
            ESP_LOGI(TAG, "MQTT_EVENT_SUBSCRIBED, msg_id=%d", event->msg_id);
            break;
        case MQTT_EVENT_UNSUBSCRIBED:
            ESP_LOGI(TAG, "MQTT_EVENT_UNSUBSCRIBED, msg_id=%d", event->msg_id);
            break;
        case MQTT_EVENT_PUBLISHED:
            ESP_LOGI(TAG, "MQTT_EVENT_PUBLISHED, msg_id=%d", event->msg_id);
            break;
        case MQTT_EVENT_DATA:
            ESP_LOGI(TAG, "MQTT_EVENT_DATA");

            cJSON *json = cJSON_ParseWithLength(event->data, event->data_len);
            cJSON *name = cJSON_GetObjectItemCaseSensitive(json, "method");
            if (cJSON_IsString(name) && (name->valuestring != NULL))
            {
                int request;
                sscanf (event->topic, RPC_REQ_FMT, &request);

                char response[512];
                snprintf(response, 512, RPC_RES_FMT, request);

                handle_request(name->valuestring, response);
            }

            break;
        case MQTT_EVENT_ERROR:
            ESP_LOGI(TAG, "MQTT_EVENT_ERROR");
            break;
        default:
            ESP_LOGI(TAG, "Other event id:%d", event->event_id);
            break;
    }
}


void mqtt_app_start(void)
{
    esp_mqtt_client_config_t mqtt_cfg = {
        .host = BROKER_IP,
        .username = TOKEN,
    };

    mqtt_client = esp_mqtt_client_init(&mqtt_cfg);
    esp_mqtt_client_register_event(mqtt_client, ESP_EVENT_ANY_ID, mqtt_event_handler, mqtt_client);
    esp_mqtt_client_start(mqtt_client);
}
