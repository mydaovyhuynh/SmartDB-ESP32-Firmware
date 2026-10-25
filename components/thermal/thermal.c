#include "thermal.h"
#include "MLX90640_API.h"
#include "esp_log.h"
#include <math.h>

static const char *TAG = "THERMAL";

#define MLX90640_ADDR 0x33
#define EMISSIVITY 0.95f

static paramsMLX90640 mlx_params;
static uint16_t frame_data[834];

esp_err_t thermal_init(void) {
    uint16_t eeprom_data[832];
    
    // Dump EEPROM and extract calibration parameters
    int status = MLX90640_DumpEE(MLX90640_ADDR, eeprom_data);
    if (status != 0) {
        ESP_LOGE(TAG, "Failed to load EEPROM");
        return ESP_FAIL;
    }
    
    status = MLX90640_ExtractParameters(eeprom_data, &mlx_params);
    if (status != 0) {
        ESP_LOGE(TAG, "Failed to extract parameters");
        return ESP_FAIL;
    }
    
    // Set refresh rate to 4Hz (from her doc)
    MLX90640_SetRefreshRate(MLX90640_ADDR, 0x03);
    
    ESP_LOGI(TAG, "MLX90640 Initialized");
    return ESP_OK;
}

esp_err_t thermal_read_frame(float *temperatures) {
    // Read both subframes (as required by her doc)
    for (int subframe = 0; subframe < 2; subframe++) {
        int status = MLX90640_GetFrameData(MLX90640_ADDR, frame_data);
        if (status < 0) {
            ESP_LOGW(TAG, "Failed to read subframe %d", subframe);
            return ESP_FAIL;
        }
        
        float ambient_temp = MLX90640_GetTa(frame_data, &mlx_params);
        MLX90640_CalculateTo(frame_data, &mlx_params, EMISSIVITY, ambient_temp, temperatures);
    }
    return ESP_OK;
}

float thermal_get_max_temp(float *temperatures) {
    float max_temp = -100.0f;
    for (int i = 0; i < 768; i++) {
        if (temperatures[i] > max_temp) {
            max_temp = temperatures[i];
        }
    }
    return max_temp;
}