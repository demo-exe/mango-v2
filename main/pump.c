#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_system.h"
#include "esp_log.h"
#include "driver/gpio.h"

#include "pump.h"
#include "nvs.h"


static const char *TAG = "pump";

#define PUMP_PIN 5 //D1
//#define PUMP_PIN 2 //D4

EventGroupHandle_t pump_event;
#define START_PUMP_REQ BIT0


void initPump()
{
    //zero-initialize the config structure.
    gpio_config_t io_conf = {};
    //disable interrupt
    io_conf.intr_type = GPIO_INTR_DISABLE;
    //set as output mode
    io_conf.mode = GPIO_MODE_OUTPUT;
    //bit mask of the pins that you want to set,e.g.GPIO18/19
    io_conf.pin_bit_mask = (1 << PUMP_PIN);
    //disable pull-down mode
    io_conf.pull_down_en = 0;
    //disable pull-up mode
    io_conf.pull_up_en = 0;
    //configure GPIO with the given settings
    gpio_config(&io_conf);

    // initally turn pump off
    gpio_set_level(PUMP_PIN, 0);
}


void waterNow()
{
    xEventGroupSetBits(pump_event, START_PUMP_REQ);
}

// most important task at priority 14 because it might flood the room if it fails.
// should not block for a long time because it will interfere with wifi task at prio 13
void waterPumpTask( void * pvParameters )
{
    pump_event = xEventGroupCreate();


    while(1) {
        EventBits_t bits = xEventGroupWaitBits(pump_event, START_PUMP_REQ, pdTRUE, pdFALSE, portMAX_DELAY);
        if (bits & START_PUMP_REQ)
        {
            ESP_LOGW(TAG, "Watering now for %dms!", getPumpingTimeMs());
            gpio_set_level(PUMP_PIN, 1);

            vTaskDelay(getPumpingTimeMs() / portTICK_PERIOD_MS);

            gpio_set_level(PUMP_PIN, 0);
            ESP_LOGW(TAG, "Watering finished!");
        }
    }
}