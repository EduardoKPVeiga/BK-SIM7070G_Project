#ifndef GSM_ROUTINE_H_
#define GSM_ROUTINE_H_

#include "freertos/FreeRTOS.h"
#include "esp_log.h"
#include "class/Gsm.h"

#define ROUTINE_DELAY 5 * 60000 / portTICK_PERIOD_MS
#define TIMER
#define PUBLISH_TIMER

void gsm_routine_task_init();

void app_routine(void *pvParameters);

#endif