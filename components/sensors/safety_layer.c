#include "safety_layer.h"
#include "freertos/queue.h"
#include "driver/gpio.h"
#include "esp_log.h"

#define RELAY_PIN GPIO_NUM_18
#define EMERGENCY_CURRENT 13.0f

static const char *TAG = "SAFETY";
extern QueueHandle_t sensor_queue; 

static void safety_layer_task(void *pvParameters) {
    // Configure Relay Pin
    gpio_config_t io_conf = {
        .pin_bit_mask = (1ULL << RELAY_PIN),
        .mode = GPIO_MODE_OUTPUT,
    };
    gpio_config(&io_conf);
    gpio_set_level(RELAY_PIN, 1); // 1 = Power ON
    
    float current_reading;
    
    while(1) {
        if (xQueueReceive(sensor_queue, &current_reading, portMAX_DELAY) == pdTRUE) {
            if (current_reading > EMERGENCY_CURRENT) {
                ESP_LOGE(TAG, "EMERGENCY! Current %.2fA. Cutting relay!", current_reading);
                gpio_set_level(RELAY_PIN, 0); // 0 = Cut Power
            }
        }
    }
}

void safety_layer_start_task(void) {
    xTaskCreatePinnedToCore(
        safety_layer_task,
        "safety_task",
        4096,
        NULL,
        10,          // Priority 10 (Highest)
        NULL,
        1            // Core 1 (Application CPU)
    );
}