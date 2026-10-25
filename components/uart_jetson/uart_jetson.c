#include "uart_jetson.h"
#include "driver/uart.h"
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include "esp_log.h"
#include <string.h>

static const char *TAG = "UART_JETSON";

#define UART_PORT UART_NUM_2
#define UART_TX_PIN GPIO_NUM_17
#define UART_RX_PIN GPIO_NUM_16
#define UART_BAUD_RATE 921600
#define UART_BUF_SIZE 2048

// Mutex to protect the UART bus from concurrent tasks
static SemaphoreHandle_t uart_mutex = NULL;

esp_err_t uart_jetson_init(void) {
    uart_config_t uart_config = {
        .baud_rate = UART_BAUD_RATE,
        .data_bits = UART_DATA_8_BITS,
        .parity    = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
    };
    
    ESP_ERROR_CHECK(uart_param_config(UART_PORT, &uart_config));
    ESP_ERROR_CHECK(uart_set_pin(UART_PORT, UART_TX_PIN, UART_RX_PIN, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE));
    ESP_ERROR_CHECK(uart_driver_install(UART_PORT, UART_BUF_SIZE, UART_BUF_SIZE, 0, NULL, 0));

    // Create the mutex
    if (uart_mutex == NULL) {
        uart_mutex = xSemaphoreCreateMutex();
    }

    ESP_LOGI(TAG, "UART2 Initialized (TX: 17, RX: 16, Baud: 921600)");
    return ESP_OK;
}

esp_err_t uart_send_frame(uint8_t *frame_data, size_t len) {
    if (uart_mutex == NULL) return ESP_ERR_INVALID_STATE;

    // Take the mutex before writing
    if (xSemaphoreTake(uart_mutex, pdMS_TO_TICKS(100)) == pdTRUE) {
        int written = uart_write_bytes(UART_PORT, frame_data, len);
        xSemaphoreGive(uart_mutex); // Release the mutex
        
        if (written == len) {
            return ESP_OK;
        }
        return ESP_FAIL;
    }
    
    ESP_LOGW(TAG, "Could not acquire UART mutex");
    return ESP_ERR_TIMEOUT;
}

esp_err_t uart_receive_score(float *score) {
    uint8_t buf[4];
    int len = uart_read_bytes(UART_PORT, buf, 4, pdMS_TO_TICKS(50));
    
    if (len == 4) {
        memcpy(score, buf, 4); // Float is 4 bytes
        return ESP_OK;
    }
    return ESP_ERR_TIMEOUT;
}
