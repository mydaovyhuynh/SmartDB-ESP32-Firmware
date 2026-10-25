#pragma once
#include "esp_err.h"

esp_err_t ads1115_init(void);
float ads1115_read_voltage(void);
float ads1115_voltage_to_current(float voltage);