#pragma once
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

typedef enum {
    STATE_BOOT,
    STATE_DISCOVERY,
    STATE_CONNECTED,
    STATE_NORMAL,
    STATE_ALERT,
    STATE_SAFE_MODE,
    STATE_RESET
} fsm_state_t;

void node_fsm_start_task(void);
void node_fsm_set_state(fsm_state_t new_state);
