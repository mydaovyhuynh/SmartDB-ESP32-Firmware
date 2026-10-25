#pragma once
#include "esp_err.h"

// Function to initialize the MLX90640
esp_err_t thermal_init(void);

// Function to read a full frame into a 768-pixel array
esp_err_t thermal_read_frame(float *temperatures);

// Function to get the maximum temperature in the current frame
float thermal_get_max_temp(float *temperatures);