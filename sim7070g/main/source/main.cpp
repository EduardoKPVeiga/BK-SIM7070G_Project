#include "../include/main.h"

static const char TAG[] = "Main";

#include "../include/class/Gsm.h"

extern "C"
{
    void app_main(void)
    {
        esp_log_level_set("*", ESP_LOG_INFO);
        ESP_LOGI(TAG, "Application Init!");

        Gsm *gsm = new Gsm("SIM7070");
        while (gsm->GetGsmErrorFlag() || gsm->GetMqttErrorFlag() || gsm->GetGpsErrorFlag())
        {
            if (gsm->GetGsmErrorFlag())
                ESP_LOGE(TAG, "GSM initialization failed!");
            else if (gsm->GetMqttErrorFlag())
                ESP_LOGE(TAG, "MQTT initialization failed!");
            else if (gsm->GetGpsErrorFlag())
                ESP_LOGE(TAG, "GPS initialization failed!");
            delete gsm;
            gsm = new Gsm("SIM7070", true);
            ESP_LOGW(TAG, "SIM7070G shutdown.");
            gsm->get_mqtt_status();
            // ESP_LOGW(TAG, "\n\nESP restart...");
            // esp_restart();
        }
        unsigned char *msg = (unsigned char *)"Hello World!";

        if (gsm == nullptr)
            ESP_LOGE(TAG, "\n\n\n\ngsm NULL.");

        while (1)
        {
            ESP_LOGI(TAG, "writing MQTT msg command...");
            gsm->mqtt_publish(msg, (size_t)strlen((const char *)msg), 0);
            vTaskDelay(5 * DELAY_ERROR_MSG);

            // MQTT_status_enum status = gsm->get_mqtt_status();
            // if (status != ON && status != ERROR)
            // {
            //     ESP_LOGE(TAG, "Disconnect time: %d", (int)((esp_timer_get_time() - gsm->connect_time) / 1000));
            //     return;
            // }
            // if (status == ERROR)
            // {
            //     ESP_LOGE(TAG, "Error time: %d", (int)((esp_timer_get_time() - gsm->connect_time) / 1000));
            //     // return;
            // }
            // vTaskDelay(1000 / portTICK_PERIOD_MS);

            // ESP_LOGI(TAG, "writing get GSM location...");
            // if (gsm->GetLocation())
            //     gsm->PrintCoord();
            // else
            //     ESP_LOGE(TAG, "failed.");
            // vTaskDelay(DELAY_ERROR_MSG);
        }
    }
}