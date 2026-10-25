#pragma once
#include "esp_err.h"

// Initialize UART2 for the Jetson Nano
esp_err_t uart_jetson_init(void);

// Send a thermal frame to the Jetson
esp_err_t uart_send_frame(uint8_t *frame_data, size_t len);

// Receive an anomaly score from the Jetson
esp_err_t uart_receive_score(float *score);

