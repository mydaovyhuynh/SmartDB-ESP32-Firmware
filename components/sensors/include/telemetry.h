#pragma once
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"

void telemetry_start_task(QueueHandle_t sensor_queue);