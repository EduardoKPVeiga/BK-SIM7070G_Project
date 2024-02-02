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
        unsigned char *msg = (unsigned char *)"Hello World!";

        while (1)
        {
            ESP_LOGI(TAG, "writing MQTT msg command...");
            // gsm->mqtt_publish("Sim7070g", (unsigned char *)"Hello", (size_t)5);
            gsm->mqtt_publish(msg, (size_t)strlen((const char *)msg), 0);
            vTaskDelay(DELAY_ERROR_MSG);

            // ESP_LOGI(TAG, "writing get GSM location...");
            // if (gsm->GetLocation())
            //     gsm->PrintCoord();
            // else
            //     ESP_LOGE(TAG, "failed.");
            // vTaskDelay(DELAY_ERROR_MSG);
        }
    }
}