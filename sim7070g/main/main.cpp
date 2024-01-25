#include "main.h"

static const char TAG[] = "Main";

// MQTT parameters
const char *broker_url = "172.104.199.107";
const char *broker_port = "1883";
const char *mqtt_topic = "Sim7070g";
const char *client_id = "15";
const char *details = "GPS data";
const bool async_mode = true;
const bool sub_hex = true;
const int qos_level = 1;

// GPRS parameters
const char *apn_vivo = "zap.vivo.com.br";
const char cid = '1';
const char *pdp_addr = "192.168.0.1";
PDP_type_enum pdp_type = IP;

// GNSS parameters
const bool gps_mode = true;
const bool plo_mode = false;
const bool bd_mode = false;
const bool gal_mode = true;
const bool qzss_mode = false;

extern "C"
{
    void app_main(void)
    {
        esp_log_level_set("*", ESP_LOG_INFO);
        ESP_LOGI(TAG, "Application Init!");

        PinsSetup();
        ESP_LOGI(TAG, "Pins Setup.");

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

        // GNSSInit();

        // while (1)
        // {
        //     while (!GetGNSS())
        //     {
        //         ESP_LOGI(TAG, "Sending get GNSS command...");
        //         vTaskDelay(10 / portTICK_PERIOD_MS);
        //     }

        //     ESP_LOGI(TAG, "Latitude:\t%f", GetLatitude());
        //     ESP_LOGI(TAG, "Longitude:\t%f", GetLongitude());
        //     ESP_LOGI(TAG, "Altitude:\t%f", GetAltitude());
        //     ESP_LOGI(TAG, "Satellites:\t%d", GetSatellitesInView());
        //     vTaskDelay(10000 / portTICK_PERIOD_MS);
        // }

        MQTTInit();

        while (!ShowNetworkSystemMode())
        {
            ESP_LOGI(TAG, "Sending show network system mode command...");
            vTaskDelay(10 / portTICK_PERIOD_MS);
        }

        // while (!PDPContext(cid, IP, apn_vivo, pdp_addr, D_OFF, H_OFF, false, false))
        // {
        //     ESP_LOGI(TAG, "Sending PDP context command...");
        //     vTaskDelay(2000 / portTICK_PERIOD_MS);
        // }
        // ESP_LOGI(TAG, "PDP context configured.");
    }
}

void GNSSInit()
{
    while (!SetGNSSPowerMode(true))
    {
        ESP_LOGI(TAG, "Sending set GNSS power mode command...");
        vTaskDelay(10 / portTICK_PERIOD_MS);
    }
    ESP_LOGI(TAG, "Power mode set.");

    while (!SetGNSSWorkMode(gps_mode, plo_mode, bd_mode, gal_mode, qzss_mode))
    {
        ESP_LOGI(TAG, "Sending set GNSS work mode command...");
        vTaskDelay(10 / portTICK_PERIOD_MS);
    }
    ESP_LOGI(TAG, "Work mode set.");
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

    while (!SetAsyncmode(async_mode))
    {
        ESP_LOGI(TAG, "Sending set asyncmode command...");
        vTaskDelay(2000 / portTICK_PERIOD_MS);
    }
    ESP_LOGI(TAG, "Asyncmode set.");

    while (!SetSubhex(sub_hex))
    {
        ESP_LOGI(TAG, "Sending set data type...");
        vTaskDelay(2000 / portTICK_PERIOD_MS);
    }
    ESP_LOGI(TAG, "Data type set.");

    while (!SetMessageDetails(details))
    {
        ESP_LOGI(TAG, "Sending message details command...");
        vTaskDelay(2000 / portTICK_PERIOD_MS);
    }
    ESP_LOGI(TAG, "Message details set.");

    while (!SetQOS(qos_level))
    {
        ESP_LOGI(TAG, "Sending set QOS command");
        vTaskDelay(10 / portTICK_PERIOD_MS);
    }
    ESP_LOGI(TAG, "Sending QOS set level command");

    while (!TestCMDMQTTParameters())
    {
        vTaskDelay(2000 / portTICK_PERIOD_MS);
    }

    while (!GetNetworkAPN())
    {
        ESP_LOGI(TAG, "Sending get network APN command...");
        vTaskDelay(10 / portTICK_PERIOD_MS);
    }
    ESP_LOGI(TAG, "Get network APN");

    vTaskDelay(5000 / portTICK_PERIOD_MS);

    while (!MQTTConnect())
    {
        ESP_LOGI(TAG, "Connecting to broker...");
        vTaskDelay(2000 / portTICK_PERIOD_MS);
    }
    ESP_LOGI(TAG, "Connected to broker.");
}

double GetLatitude()
{
    int cont_comma = 0;
    char latitude[11] = {'\0'};
    for (int i = SIZE(CGNSINF) + 2, j = 0; i < strlen(msg_received); i++)
    {
        if (msg_received[i] == ',')
            cont_comma++;
        if (cont_comma == 3 && msg_received[i] != ',')
        {
            latitude[j] = msg_received[i];
            j++;
        }
    }
    return atof(latitude);
}

double GetLongitude()
{
    int cont_comma = 0;
    char longitude[12] = {'\0'};
    for (int i = SIZE(CGNSINF) + 2, j = 0; i < strlen(msg_received); i++)
    {
        if (msg_received[i] == ',')
            cont_comma++;
        if (cont_comma == 4 && msg_received[i] != ',')
        {
            longitude[j] = msg_received[i];
            j++;
        }
    }
    return atof(longitude);
}

double GetAltitude()
{
    int cont_comma = 0;
    char altitude[9] = {'\0'};
    for (int i = SIZE(CGNSINF) + 2, j = 0; i < strlen(msg_received); i++)
    {
        if (msg_received[i] == ',')
            cont_comma++;
        if (cont_comma == 5 && msg_received[i] != ',')
        {
            altitude[j] = msg_received[i];
            j++;
        }
    }
    return atof(altitude);
}

int GetSatellitesInView()
{
    int cont_comma = 0;
    char satellites[3] = {'\0'};
    for (int i = SIZE(CGNSINF) + 2, j = 0; i < strlen(msg_received); i++)
    {
        if (msg_received[i] == ',')
            cont_comma++;
        if (cont_comma == 14 && msg_received[i] != ',')
        {
            satellites[j] = msg_received[i];
            j++;
        }
    }
    return atof(satellites);
}