#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "sim7070g_commands.h"
#include "esp_log.h"
#include "uart_sim7070g.h"

static const char TAG[] = "Main";

extern "C"
{
    void app_main(void)
    {
        esp_log_level_set("*", ESP_LOG_INFO);
        ESP_LOGI(TAG, "Application Init!");

        PinsSetup();
        ESP_LOGI(TAG, "Pins Setup.");

        // xTaskCreatePinnedToCore(&UartTask, "Uart Task", UART_TASK_STACK_SIZE, NULL, UART_TASK_PRIORITY, NULL, UART_TASK_CORE_ID);
        // ESP_LOGI(TAG, "Uart Task created.");
        UARTSim7070gInit();
        ESP_LOGI(TAG, "UART SIM7070G Init.");
        vTaskDelay(5000 / portTICK_PERIOD_MS);

        PWRKEYPulse();
        ESP_LOGI(TAG, "SIM7070G Init.");

        while (!EchoBackOff())
        {
            ESP_LOGI(TAG, "Sending echo command...");
            vTaskDelay(10 / portTICK_PERIOD_MS);
        }
        ESP_LOGI(TAG, "Echo mode disabled.");

        while (!SetClientID("15", 2))
        {
            ESP_LOGI(TAG, "Sending client ID...");
            vTaskDelay(2000 / portTICK_PERIOD_MS);
        }
        ESP_LOGI(TAG, "Set client ID.");

        while (!SetBrokerURL("172.104.199.107", "1883"))
        {
            ESP_LOGI(TAG, "Sending broker URL...");
            vTaskDelay(2000 / portTICK_PERIOD_MS);
        }
        ESP_LOGI(TAG, "Set broker URL.");

        while (!MQTTSubscribeTopic("Sim7070"))
        {
            ESP_LOGI(TAG, "Subscrinbing MQTT topic...");
            vTaskDelay(2000 / portTICK_PERIOD_MS);
        }
        ESP_LOGI(TAG, "Topic subscribe.");

        while (!TestCMDMQTTParameters())
        {
            vTaskDelay(2000 / portTICK_PERIOD_MS);
        }

        while (!MQTTConnect())
        {
            vTaskDelay(2000 / portTICK_PERIOD_MS);
        }
        ESP_LOGI(TAG, "Connected to broker.");
    }
}