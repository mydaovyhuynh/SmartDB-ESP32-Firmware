#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "wifi_manager.h"
#include "i2c_init.h"
#include "ads1115.h"
#include "safety_layer.h"
#include "sensor_sampling.h"
#include "telemetry.h"  
#include "node_fsm.h" 
#include "thermal.h" 
#include "uart_jetson.h"
static const char *TAG = "app_main";
QueueHandle_t sensor_queue;

void app_main(void)
{
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        nvs_flash_erase();
        nvs_flash_init();
    }

    ESP_LOGI(TAG, "SmartDB firmware starting");

    ESP_ERROR_CHECK(i2c_master_init());
    ESP_ERROR_CHECK(ads1115_init());
    //Initialize thermal camera
    if (thermal_init() == ESP_OK) {
        ESP_LOGI(TAG, "thermal camera is ready");
    }else{ ESP_LOGE(TAG, "Thermal camera is failed ti init");
    }
    ESP_ERROR_CHECK(uart_jetson_init()); 
    sensor_queue = xQueueCreate(5, sizeof(float));

    // Start Tasks
    safety_layer_start_task();
    sensor_sampling_start_task(sensor_queue);
    telemetry_start_task(sensor_queue); 
    
    node_fsm_start_task(); 
    wifi_manager_init();

    while (1) {
        vTaskDelay(pdMS_TO_TICKS(10000));
    }
}