typedef enum {
    STATE_BOOT,
    STATE_DISCOVERY,
    STATE_CONNECTED,
    STATE_NORMAL,
    STATE_ALERT,
    STATE_SAFE_MODE,
    STATE_RESET
} fsm_state_t;

void fsm_task(void *pvParameters) {
    fsm_state_t current_state = STATE_BOOT;
    
    while(1) {
        switch(current_state) {
            case STATE_BOOT:
                // Initialize NVS, I2C, Relay...
                current_state = STATE_DISCOVERY;
                break;
            case STATE_DISCOVERY:
                // Try to connect to WiFi
                // If connected -> STATE_CONNECTED
                break;
            case STATE_NORMAL:
                // Normal operation
                break;
            case STATE_ALERT:
                // Send alert to Firebase
                break;
            default:
                break;
        }
        vTaskDelay(pdMS_TO_TICKS(100));
    }
}