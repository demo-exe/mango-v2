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


static const char *TAG = "nvs";

static nvs_handle_t my_nvs_handle;
static uint32_t pumpingTimeMs = 0;


uint32_t getPumpingTimeMs()
{
    if(pumpingTimeMs == 0)
    {
        pumpingTimeMs = 2000; // value will default to 2000, if not set yet in NVS

        // Read
        esp_err_t err = nvs_get_u32(my_nvs_handle, "pumpingTimeMs", &pumpingTimeMs);
        switch (err) {
            case ESP_OK:
                ESP_LOGI(TAG, "Read pumpingTimeMs from nvs: %d", pumpingTimeMs);
                break;
            case ESP_ERR_NVS_NOT_FOUND:
                ESP_LOGI(TAG, "Initializing pumpingTimeMs in nvs: %d", pumpingTimeMs);
                break;
            default :
                ESP_LOGE(TAG, "Error (%s) reading!\n", esp_err_to_name(err));
        }
    }
    
    return pumpingTimeMs;
}

void setPumpingTimeMs(unsigned int ms)
{
    pumpingTimeMs = ms;

    esp_err_t err = nvs_set_u32(my_nvs_handle, "pumpingTimeMs", ms);
    if (err != ESP_OK)
    {
        ESP_LOGE(TAG, "Error (%s) setting pumpingTimeMs!\n", esp_err_to_name(err));
    }

    err = nvs_commit(my_nvs_handle);
    if (err != ESP_OK)
    {
        ESP_LOGE(TAG, "Error (%s) comitting pumpingTimeMs!\n", esp_err_to_name(err));
    }
}


void initNVS()
{
    ESP_ERROR_CHECK(nvs_flash_init());

    esp_err_t err = nvs_open("storage", NVS_READWRITE, &my_nvs_handle);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Error (%s) opening NVS handle!\n", esp_err_to_name(err));
    } else {
        ESP_LOGI(TAG, "Opened NVS");
    }
}

