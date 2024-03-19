#ifndef GSM_ROUTINE_H_
#define GSM_ROUTINE_H_

#include "freertos/FreeRTOS.h"
#include "esp_log.h"
#include "class/Gsm.h"

#define ROUTINE_DELAY (1 / 2) * 60000 / portTICK_PERIOD_MS

void gsm_routine_task_init();

void app_routine(void *pvParameters);

#endif