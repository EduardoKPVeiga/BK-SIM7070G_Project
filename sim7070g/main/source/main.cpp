#include "../include/main.h"

static const char TAG[] = "Main";

unsigned char *msg = (unsigned char *)"Hello World!";

extern "C"
{
    void app_main(void)
    {
        esp_log_level_set("*", ESP_LOG_INFO);
        uart2_task_init();
        Gsm *gsm = new Gsm("NrAAFfn");
        // gsm->Initialize();

        while (1)
        {
            app_routine(gsm);
            vTaskDelay(100 / portTICK_PERIOD_MS);
        }
    }
}

void app_routine(Gsm *gsm)
{
    ESP_LOGI(TAG, "Routine Init!");
    gsm->PowerOn();

    if (gsm->GetGsmErrorFlag())
        ESP_LOGE(TAG, "GSM initialization failed!");
    else if (gsm->GetMqttErrorFlag())
        ESP_LOGE(TAG, "MQTT initialization failed!");
    else if (gsm->GetGpsErrorFlag())
        ESP_LOGE(TAG, "GPS initialization failed!");

    // gsm->mqtt_sub("NrAAFfn/0/msg");
    // gsm->mqtt_publish(msg, (size_t)strlen((const char *)msg), 0);

    // if (!gsm->GetGpsErrorFlag())
    // {
    //     for (int i = 0; i < 5; i++)
    //     {
    //         if (gsm->GetLocation())
    //             gsm->PrintCoord();
    //         else
    //             ESP_LOGE(TAG, "Get location failed!");
    //     }
    // }

    gsm->PowerOff();
    vTaskDelay(100 / portTICK_PERIOD_MS);
}