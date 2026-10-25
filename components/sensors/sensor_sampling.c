#include "sensor_sampling.h"
#include "ads1115.h"
#include "freertos/task.h"
#include "esp_log.h"

static const char *TAG = "SAMPLING";
static QueueHandle_t s_queue;

static void sensor_sampling_task(void *pvParameters) {
    ESP_LOGI(TAG, "Sensor Sampling Task Started (100ms cycle)");
    
    while(1) {
        float voltage = ads1115_read_voltage();
        float current = ads1115_voltage_to_current(voltage);
        
        // Send data to the queue for the Safety Layer and Telemetry
        xQueueSend(s_queue, &current, pdMS_TO_TICKS(10));
        
        vTaskDelay(pdMS_TO_TICKS(100)); // 100ms cycle from doc
    }
}

void sensor_sampling_start_task(QueueHandle_t queue) {
    s_queue = queue;
    xTaskCreatePinnedToCore(
        sensor_sampling_task,
        "sensor_task",
        4096,
        NULL,
        5,           // Priority 5
        NULL,
        1            // Core 1
    );
}