#include "main.h"

static const char TAG[] = "Main";

const char *broker_url = "172.104.199.107";
const char *broker_port = "1883";
const char *mqtt_topic = "Sim7070g";
const char *client_id = "15";

const char *apn_vivo = "zap.vivo.com.br";
const char cid = '1';
const char *pdp_addr = "192.168.0.1";
PDP_type_enum pdp_type = IP;

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

        // MQTTInit();

        // while (!PingIPV4(broker_url))
        // {
        //     ESP_LOGI(TAG, "Ping...");
        //     vTaskDelay(2000 / portTICK_PERIOD_MS);
        // }
        // ESP_LOGI(TAG, "Sucessful communication.");

        while (!PDPContext(cid, pdp_type, apn_vivo, pdp_addr))
        {
            ESP_LOGI(TAG, "Sending PDP context command...");
            vTaskDelay(2000 / portTICK_PERIOD_MS);
        }
        ESP_LOGI(TAG, "PDP context configured.");
    }
}

void MQTTInit()
{
    while (!SetClientID(client_id))
    {
        ESP_LOGI(TAG, "Sending client ID...");
        vTaskDelay(2000 / portTICK_PERIOD_MS);
    }
    ESP_LOGI(TAG, "Set client ID.");

    while (!SetBrokerURL(broker_url, broker_port))
    {
        ESP_LOGI(TAG, "Sending broker URL...");
        vTaskDelay(2000 / portTICK_PERIOD_MS);
    }
    ESP_LOGI(TAG, "Set broker URL.");

    while (!MQTTSubscribeTopic(mqtt_topic))
    {
        ESP_LOGI(TAG, "Subscrinbing MQTT topic...");
        vTaskDelay(2000 / portTICK_PERIOD_MS);
    }
    ESP_LOGI(TAG, "Topic subscribe.");

    while (!TestCMDMQTTParameters())
    {
        vTaskDelay(2000 / portTICK_PERIOD_MS);
    }

    vTaskDelay(5000 / portTICK_PERIOD_MS);

    while (!MQTTConnect())
    {
        ESP_LOGI(TAG, "Connecting to broker...");
        vTaskDelay(2000 / portTICK_PERIOD_MS);
    }
    ESP_LOGI(TAG, "Connected to broker.");
}