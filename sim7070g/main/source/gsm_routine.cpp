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

    for (;;)
    {
        gsm->PowerOn();

        if (gsm->GetGsmErrorFlag())
            ESP_LOGE(TAG, "GSM initialization failed!");
        else if (gsm->GetMqttErrorFlag())
            ESP_LOGE(TAG, "MQTT initialization failed!");
        else if (gsm->GetGpsErrorFlag())
            ESP_LOGE(TAG, "GPS initialization failed!");

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
            if (gsm->GetLocation())
                gsm->PrintCoord();
            else
                ESP_LOGE(TAG, "Get location failed!");
        }
#endif

        gsm->PowerOff();
        vTaskDelay(ROUTINE_DELAY);
    }
}