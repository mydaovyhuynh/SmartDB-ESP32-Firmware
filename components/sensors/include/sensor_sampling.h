#pragma once
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"

void sensor_sampling_start_task(QueueHandle_t queue);