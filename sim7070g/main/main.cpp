#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "sim7070g_commands.h"
#include "esp_log.h"

static const char TAG[] = "main";

const uart_port_t uart_0 = UART_NUM_0;

extern "C"
{
    void app_main(void)
    {
        esp_log_level_set("*", ESP_LOG_INFO);

        ESP_LOGI(TAG, "Application Init!");

        PinsSetup();

        ESP_LOGI(TAG, "Pins Setup.");

        PWRKEYPulse();

        ESP_LOGI(TAG, "SIM7070G Init.");

        UARTSim7070gInit();
        ESP_LOGI(TAG, "UART SIM7070G Init.");

        while (1)
        {
            TestCMDMQTTParameters();
            char msg[message_pointer_pos + 1] = {0};
            for (int i = 0; i < message_pointer_pos; i++)
            {
                msg[i] = message_buff[i];
            }
            msg[message_pointer_pos] = '\0';
            ESP_LOGI(TAG, "%s", msg);
            vTaskDelay(5000 / portTICK_PERIOD_MS);
        }
    }
}