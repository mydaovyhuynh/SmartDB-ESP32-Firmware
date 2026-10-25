#include "node_fsm.h"
#include "esp_log.h"
#include "wifi_manager.h"
#include "freertos/queue.h"

static const char *TAG = "NODE_FSM";
static fsm_state_t current_state = STATE_BOOT;

// We will use a queue to tell the FSM to change states safely
static QueueHandle_t fsm_event_queue;

static void node_fsm_task(void *pvParameters) {
    fsm_state_t incoming_state;
    
    ESP_LOGI(TAG, "FSM Task Started (Priority 7, Core 1)");

    while (1) {
        // Check if there's a new state requested by another task
        if (xQueueReceive(fsm_event_queue, &incoming_state, pdMS_TO_TICKS(100)) == pdTRUE) {
            ESP_LOGI(TAG, "State transition: %d -> %d", current_state, incoming_state);
            current_state = incoming_state;
        }

        // Handle the current state
        switch (current_state) {
            case STATE_BOOT:
                // Hardware is already initialized in main.c, move to DISCOVERY
                current_state = STATE_DISCOVERY; 
                break;

            case STATE_DISCOVERY:
                // Wait for WiFi to connect
                if (wifi_manager_is_connected()) {
                    current_state = STATE_CONNECTED;
                }
                break;

            case STATE_CONNECTED:
                // Once connected, we are ready to monitor
                current_state = STATE_NORMAL;
                break;

            case STATE_NORMAL:
                // Normal operation. The Safety Task handles alerts if current spikes.
                // If we lose WiFi, go back to discovery
                if (!wifi_manager_is_connected()) {
                    current_state = STATE_DISCOVERY;
                }
                break;

            case STATE_ALERT:
                // Send alert to cloud (handled by telemetry later)
                ESP_LOGW(TAG, "FSM in ALERT state!");
                break;

            case STATE_SAFE_MODE:
                // Relay is cut. Wait for manual reset.
                ESP_LOGE(TAG, "FSM in SAFE MODE. Power is cut.");
                break;

            case STATE_RESET:
                ESP_LOGI(TAG, "FSM Resetting...");
                current_state = STATE_BOOT;
                break;
                
            default:
                break;
        }
        
        vTaskDelay(pdMS_TO_TICKS(100)); // Don't block the CPU
    }
}

void node_fsm_start_task(void) {
    fsm_event_queue = xQueueCreate(5, sizeof(fsm_state_t));
    
    xTaskCreatePinnedToCore(
        node_fsm_task,
        "fsm_task",
        3072,
        NULL,
        7,           // Priority 7
        NULL,
        1            // Core 1
    );
}

// Call this from anywhere (e.g., Safety Task) to change state
void node_fsm_set_state(fsm_state_t new_state) {
    if (fsm_event_queue != NULL) {
        xQueueSend(fsm_event_queue, &new_state, pdMS_TO_TICKS(10));
    }
}