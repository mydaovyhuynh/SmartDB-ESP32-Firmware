#include "telemetry.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "wifi_manager.h"
#include <stdio.h>

static const char *TAG = "TELEMETRY";

static void telemetry_task(void *pvParameters) {
    QueueHandle_t queue = (QueueHandle_t)pvParameters;
    float current_reading;
    
    ESP_LOGI(TAG, "Telemetry Task Started (Priority 3, Core 0)");

    while (1) {
        // Wait for data from the sensor queue
        if (xQueueReceive(queue, &current_reading, portMAX_DELAY) == pdTRUE) {
            
            if (wifi_manager_is_connected()) {
                // In the future, this is where you will send the data to Firebase 
                // or the Jetson Nano via DTLS.
                // For now, we just log it to prove the architecture works.
                
                ESP_LOGI(TAG, "Telemetry -> Current: %.3f A | Sent to Cloud", current_reading);
            } else {
                ESP_LOGW(TAG, "WiFi down. Data not sent.");
            }
        }
    }
}

void telemetry_start_task(QueueHandle_t sensor_queue) {
    xTaskCreatePinnedToCore(
        telemetry_task,
        "telemetry_task",
        4096,
        (void*)sensor_queue,
        3,           // Priority 3 (Lower priority, can be delayed)
        NULL,
        0            // Core 0 (Protocol CPU)
    );
}