#include "../include/gsm_routine.h"

static const char TAG[] = "GSM_ROUTINE";

unsigned char *msg = (unsigned char *)"Hello World!";

void gsm_routine_task_init()
{
    xTaskCreatePinnedToCore(&app_routine, "app routine", 4 * MSG_RECEIVED_BUFF_SIZE, nullptr, 20, &tunnelTaskHandle, 0);
}

void app_routine(void *pvParameters)
{
    ESP_LOGI(TAG, "App Routine init.");
    Gsm *gsm = new Gsm("NrAAFfn");
    ESP_LOGI(TAG, "Gsm object created.");
    int64_t timer = 0;

    for (;;)
    {
        timer = esp_timer_get_time();
        gsm->PowerOn();

#ifdef MQTT
        if (!gsm->GetMqttErrorFlag())
        {
            for (int i = 0; i < 3; i++)
            {
                // gsm->mqtt_sub("NrAAFfn/0/msg");
                if (gsm->mqtt_publish(msg, (size_t)strlen((const char *)msg), 0))
                    break;
                vTaskDelay(2000 / portTICK_PERIOD_MS);
            }
        }
#endif

#ifdef GPS
        if (!gsm->GetGpsErrorFlag())
        {
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
        }
#endif

        gsm->PowerOff();
        ESP_LOGI(TAG, "Time: %f s", (esp_timer_get_time() - timer) / 1000000.f);
        vTaskDelay(ROUTINE_DELAY);
    }
}