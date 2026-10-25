#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "driver/gpio.h"
#include "esp_log.h"

#define RELAY_PIN GPIO_NUM_18
#define EMERGENCY_CURRENT 13.0f

// This queue will be defined in your main file later
extern QueueHandle_t sensor_queue; 

void safety_layer_task(void *pvParameters) {
    float current_reading;
    
    while(1) {
        // Wait for data from the sensor task
        if (xQueueReceive(sensor_queue, &current_reading, portMAX_DELAY) == pdTRUE) {
            
            // If current is too high, cut power immediately
            if (current_reading > EMERGENCY_CURRENT) {
                ESP_LOGE("SAFETY", "EMERGENCY! Current %.2fA. Cutting relay!", current_reading);
                gpio_set_level(RELAY_PIN, 0); // Cut relay
            }
        }
    }
}