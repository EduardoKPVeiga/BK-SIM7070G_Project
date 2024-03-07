#include "../include/main.h"

static const char TAG[] = "Main";

#include "../include/class/Gsm.h"

Gsm *gsm = nullptr;

extern "C"
{
    void app_main(void)
    {
        esp_log_level_set("*", ESP_LOG_INFO);
        uart2_task_init();

        gsm = new Gsm(client_id);

        while (1)
        {
            app_routine();
            vTaskDelay(60000 / portTICK_PERIOD_MS);
        }
    }
}

void app_routine()
{
    ESP_LOGI(TAG, "Routine Init!");

    if (gsm->GetGsmErrorFlag())
    {
        ESP_LOGE(TAG, "GSM initialization failed!");
        delete gsm;
        gsm = nullptr;
    }
    else if (gsm->GetMqttErrorFlag())
    {
        ESP_LOGE(TAG, "MQTT initialization failed!");
        delete gsm;
        gsm = nullptr;
    }
    else if (gsm->GetGpsErrorFlag())
    {
        ESP_LOGE(TAG, "GPS initialization failed!");
        delete gsm;
        gsm = nullptr;
    }

    unsigned char *msg = (unsigned char *)"Hello World!";

    if (gsm == nullptr)
    {
        ESP_LOGE(TAG, "\n\n\n\ngsm NULL.");
        gsm = new Gsm(client_id);
    }

    // gsm->mqtt_sub("NrAAFfn/0/msg");

    PWRKEYPowerOff();
    ESP_LOGI(TAG, "Power OFF.");

    /*
    while (1)
    {
        ESP_LOGI(TAG, "writing MQTT msg command...");
        // ESP_LOGI(TAG, "Network active: %d", AppNetworkActiveReadCMD(0));
        gsm->mqtt_publish(msg, (size_t)strlen((const char *)msg), 0);
        vTaskDelay(50 * DELAY_ERROR_MSG);
        // GPS test
        // ESP_LOGI(TAG, "writing get GSM location...");
        // if (gsm->GetLocation())
        //     gsm->PrintCoord();
        // else
        //     ESP_LOGE(TAG, "failed.");
        // vTaskDelay(DELAY_ERROR_MSG);
    }
    //*/
}