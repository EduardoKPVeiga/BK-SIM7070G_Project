#include "../include/gsm_routine.h"

static const char TAG[] = "GSM_ROUTINE";

unsigned char *msg = (unsigned char *)"Hello World! Testing GSM module...";

void gsm_routine_task_init()
{
    xTaskCreatePinnedToCore(&app_routine, "app routine", 4 * MSG_RECEIVED_BUFF_SIZE, nullptr, 20, &tunnelTaskHandle, 0);
}

void app_routine(void *pvParameters)
{
    ESP_LOGI(TAG, "App Routine init.");
    Gsm *gsm = new Gsm("NrAAFfn");
    ESP_LOGI(TAG, "Gsm object created.");

#ifdef TIMER
    int64_t timer = 0;
    int64_t time_spent = 0;
#ifdef PUBLISH_TIMER
    char time_msg[10] = {'\0'};
    uint8_t msg_size = 0;
#endif
#endif

    for (;;)
    {
#ifdef TIMER
        timer = esp_timer_get_time();
#endif
        if (gsm->PowerOn())
        {
#ifdef MQTT
#ifdef TIMER
            ESP_LOGI(TAG, "time_spent: %d s", (int)((time_spent / 1000000.f) + 0.5f));
#ifdef PUBLISH_TIMER
            msg_size = DecimalToStr((uint16_t)((time_spent / 1000000.f) + 0.5f), time_msg, 10);
            ESP_LOGI(TAG, "time_msg: %s", time_msg);
#endif
#endif
            for (int i = 0; i < 3; i++)
            {
// gsm->mqtt_sub("NrAAFfn/0/msg");
#ifdef TIMER
#ifdef PUBLISH_TIMER
                if (gsm->mqtt_publish((unsigned char *)time_msg, msg_size, 0))
#else
                if (gsm->mqtt_publish((unsigned char *)msg, (size_t)strlen((const char *)msg), 0))
#endif
#endif
                    break;
                vTaskDelay(1000 / portTICK_PERIOD_MS);
            }
#endif

#ifdef GPS
            int sat = 0;
            while (sat <= 0)
            {
                if (gsm->GetLocation())
                {
                    gsm->PrintCoord();
                    sat = gsm->GetSatellitesInView();
                }
                else
                    ESP_LOGE(TAG, "Get location failed!");
                vTaskDelay(1000 / portTICK_PERIOD_MS);
            }
#endif
        }

        gsm->PowerOff();
#ifdef TIMER
        time_spent = (esp_timer_get_time() - timer);
        ESP_LOGI(TAG, "Time: %f s", time_spent / 1000000.f);
#endif
        vTaskDelay(ROUTINE_DELAY);
    }
}