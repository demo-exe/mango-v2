#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_log.h"
#include "driver/gpio.h"



static const char *TAG = "pump";

#define PUMP_PIN 2 //D4


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

    gpio_set_level(PUMP_PIN, 0);
}


// most important task at priority 14 because it might flood the room if it fails.
// should not block for a long time because it will interfere with wifi task at prio 13
void waterPumpTask( void * pvParameters )
{
    int cnt = 0;
    while(1) {
        cnt++;
        gpio_set_level(PUMP_PIN, cnt % 2);
        //ESP_LOGI(TAG, "pump task 1s");
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}