#include "../include/main.h"

static const char TAG[] = "Main";

unsigned char *msg = (unsigned char *)"Hello World!";

extern "C"
{
    void app_main(void)
    {
        esp_log_level_set("*", ESP_LOG_INFO);
        uart2_task_init();
        xTaskCreatePinnedToCore(&app_routine, "app routine", 4 * MSG_RECEIVED_BUFF_SIZE, nullptr, 20, &tunnelTaskHandle, 0);
    }
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
        vTaskDelay(STACK_OF_DELAY);
    }
}