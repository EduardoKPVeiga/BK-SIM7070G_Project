#include "../include/main.h"

static const char TAG[] = "Main";

#include "../include/class/Gsm.h"

extern "C"
{
    void app_main(void)
    {
        esp_log_level_set("*", ESP_LOG_INFO);
        ESP_LOGI(TAG, "Application Init!");

        Gsm *gsm = new Gsm(client_id);
        while (gsm->GetGsmErrorFlag() || gsm->GetMqttErrorFlag() || gsm->GetGpsErrorFlag())
        {
            if (gsm->GetGsmErrorFlag())
                ESP_LOGE(TAG, "GSM initialization failed!");
            else if (gsm->GetMqttErrorFlag())
                ESP_LOGE(TAG, "MQTT initialization failed!");
            else if (gsm->GetGpsErrorFlag())
                ESP_LOGE(TAG, "GPS initialization failed!");
            delete gsm;
            ESP_LOGW(TAG, "SIM7070G shutdown.");
            gsm = new Gsm(client_id, true);
            gsm->get_mqtt_status();
        }
        unsigned char *msg = (unsigned char *)"Hello World!";

        if (gsm == nullptr)
            ESP_LOGE(TAG, "\n\n\n\ngsm NULL.");

        /*
        while (1)
        {
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
}