#ifndef WIFI_H_
#define WIFI_H_

#include "wifi_pass.h"
// #define EXAMPLE_ESP_WIFI_SSID      "Pfizer control beacon"
// #define EXAMPLE_ESP_WIFI_PASS      "WZtTdCxYeXFLKuECbn19qsHnp"
#define EXAMPLE_ESP_MAXIMUM_RETRY  3

/* FreeRTOS event group to signal when we are connected*/
extern EventGroupHandle_t s_wifi_event_group;

/* The event group allows multiple bits for each event, but we only care about two events:
 * - we are connected to the AP with an IP
 * - we failed to connect after the maximum amount of retries */
#define WIFI_DISCONNECTED_BIT BIT0
#define WIFI_CONNECTED_BIT BIT1


void wifiTask(void * pvParameters);

#endif // WIFI_H_