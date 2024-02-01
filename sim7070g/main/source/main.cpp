#include "../include/main.h"

static const char TAG[] = "Main";

uint8_t cmd_failed_counter = 0;

int pdp = 0;
const char ip_type = '1'; // Internet Protocol Version 4

// MQTT parameters
const char *broker_url = "172.104.199.107";
const char *broker_port = "1883";
const char *mqtt_topic = "Sim7070g";
const char *client_id = "15";
const char *details = "GPS data";
const bool async_mode = true;
const bool sub_hex = false;
const Qos_enum qos_level = QOS_1;

// GPRS parameters
const char *apn_vivo = "zap.vivo.com.br";
const char cid = '1';
const char *pdp_addr = "";
const char *pdp_type = "IP";

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

        uart2_task_init();

        PWRKEYPulse();
        ESP_LOGI(TAG, "SIM7070G Init.");

        while (!EchoBackOff())
        {
            ESP_LOGI(TAG, "Sending echo command...");
            vTaskDelay(DELAY_ERROR_MSG);
        }
        ESP_LOGI(TAG, "Echo mode disabled.\n");

        // GNSSInit();

        PDNManualActivation();

        while (!PingIPV4(broker_url))
        {
            ESP_LOGI(TAG, "Sending ping command...");
            vTaskDelay(5000 / portTICK_PERIOD_MS);
        }
        ESP_LOGI(TAG, "Ping IPV4 send.\n");

        // while (1)
        // {
        //     ESP_LOGI(TAG, "Writing get gsm location command...");
        //     if (GetGSMLocation(1, '0'))
        //         ESP_LOGI(TAG, "Get location.");
        //     else
        //         ESP_LOGE(TAG, "failed.");
        //     vTaskDelay(DELAY_ERROR_MSG * 2);
        // }

        MQTTInit();
        while (1)
        {
            ESP_LOGI(TAG, "Sending MQTT msg command...");
            SendPacket("Sim7070g", "5", QOS_0, 1, "Hello");
            vTaskDelay(DELAY_ERROR_MSG);
        }
    }
}

void PDNAutoActivation()
{
    ESP_LOGI(TAG, "Sending check SIM card command...");
    while (!CheckSIMCard())
    {
        ESP_LOGI(TAG, "Sending check SIM card command...");
        vTaskDelay(DELAY_ERROR_MSG);
    }
    if (StrContainsSubstr(msg_received, "READY", MSG_RECEIVED_BUFF_SIZE, strlen("READY")))
        ESP_LOGI(TAG, "SIM card READY.\n");
    else
        ESP_LOGE(TAG, "SIM card error.\n");

    ESP_LOGI(TAG, "Sending set network type command...");
    while (!SetNetworkType(NB_IOT))
    {
        ESP_LOGI(TAG, "Sending set network type command...");
        vTaskDelay(DELAY_ERROR_MSG);
    }
    ESP_LOGI(TAG, "Network selected.\n");

    ESP_LOGI(TAG, "Sending check RF signal command...");
    while (!CheckRF())
    {
        ESP_LOGI(TAG, "Sending check RF signal command...");
        vTaskDelay(2000 / portTICK_PERIOD_MS);
    }

    ESP_LOGI(TAG, "Sending check PS service command...");
    while (!CheckPSService())
    {
        ESP_LOGI(TAG, "Sending check PS service command...");
        vTaskDelay(DELAY_ERROR_MSG);
    }

    ESP_LOGI(TAG, "Sending query nertwork information command...");
    while (!QueryNetworkInfo())
    {
        ESP_LOGI(TAG, "Sending query nertwork information command...");
        vTaskDelay(DELAY_ERROR_MSG);
    }

    ESP_LOGI(TAG, "Sending get nertwork APN command...");
    while (!GetNetworkAPN())
    {
        ESP_LOGI(TAG, "Sending get nertwork APN command...");
        vTaskDelay(DELAY_ERROR_MSG);
    }

    ESP_LOGI(TAG, "Sending PDP configure command...");
    while (!PDPConfigure(pdp, ip_type, apn_vivo))
    {
        ESP_LOGI(TAG, "Sending PDP configure command...");
        vTaskDelay(DELAY_ERROR_MSG);
    }
    PDPConfigureReadCMD();
    ESP_LOGI(TAG, "PDP configured.\n");

    ESP_LOGI(TAG, "Sending set EPS network status command...");
    while (!SetEPSNetworkStatus('1'))
    {
        ESP_LOGI(TAG, "Sending set EPS network status command...");
        vTaskDelay(DELAY_ERROR_MSG);
    }
    ESP_LOGI(TAG, "Set EPS network status.\n");

    // while (!BandTest())
    // {
    //     ESP_LOGI(TAG, "Sending band test command...");
    //     vTaskDelay(DELAY_ERROR_MSG);
    // }

    ESP_LOGI(TAG, "Sending APP network active command...");
    while (!APPNetworkActive(pdp, ACTIVED))
    {
        AppNetworkActiveReadCMD(pdp);
        ESP_LOGI(TAG, "Sending APP network active command...");
        vTaskDelay(2000 / portTICK_PERIOD_MS);
    }
    ESP_LOGI(TAG, "APP network actived.\n");
}

void PDNManualActivation()
{
    // Disable RF
    ESP_LOGI(TAG, "writing set phone functionality command...");
    while (!SetPhoneFunc(MIN_FUNC))
    {
        ESP_LOGI(TAG, "writing set phone functionality command...");
        vTaskDelay(DELAY_ERROR_MSG);
    }
    ESP_LOGI(TAG, "Set phone.\n");

    // Set the APN manually. Some operators needtoset APN first when registering the network.
    ESP_LOGI(TAG, "writing set PDP context command...");
    while (!PDPContext(cid, pdp_type, apn_vivo))
    {
        ESP_LOGI(TAG, "writing set PDP context command...");
        vTaskDelay(DELAY_ERROR_MSG);
    }
    ESP_LOGI(TAG, "Set PDP context.\n");

    // Enable RF
    ESP_LOGI(TAG, "writing set phone functionality command...");
    while (!SetPhoneFunc(FULL_FUNC))
    {
        ESP_LOGI(TAG, "writing set phone functionality command...");
        vTaskDelay(DELAY_ERROR_MSG);
    }
    ESP_LOGI(TAG, "Set phone.\n");

    // Check PS service. 1 indicates PS has attached.
    ESP_LOGI(TAG, "writing check PS service command...");
    while (!CheckPSService())
    {
        ESP_LOGI(TAG, "Sending check PS service command...");
        vTaskDelay(DELAY_ERROR_MSG);
    }

    // Query the APN delivered by the network after theCAT-M or NB-IOT network is successfullyregistered
    ESP_LOGI(TAG, "writing get nertwork APN command...");
    while (!GetNetworkAPN())
    {
        ESP_LOGI(TAG, "Sending get nertwork APN command...");
        vTaskDelay(DELAY_ERROR_MSG);
    }

    // Before activation please use AT+CNCFGtoset APN\user name\password if needed.
    ESP_LOGI(TAG, "writing PDP configure command...");
    while (!PDPConfigure(pdp, ip_type, apn_vivo))
    {
        ESP_LOGI(TAG, "writing PDP configure command...");
        vTaskDelay(DELAY_ERROR_MSG);
    }
    PDPConfigureReadCMD();
    ESP_LOGI(TAG, "PDP configured.\n");

    ESP_LOGI(TAG, "writing get band scan configuration command...");
    while (!GetBandScanConfig())
    {
        ESP_LOGI(TAG, "writing get band scan configuration command...");
        vTaskDelay(DELAY_ERROR_MSG);
    }

    ESP_LOGI(TAG, "writing get engineering mode information command...");
    while (!GetEngineeringModeInfo())
    {
        ESP_LOGI(TAG, "writing get engineering mode information command...");
        vTaskDelay(DELAY_ERROR_MSG);
    }

    // Activate network, Activate 0th PDP.
    ESP_LOGI(TAG, "writing APP network active command...");
    while (!APPNetworkActive(pdp, ACTIVED))
    {
        ESP_LOGI(TAG, "writing APP network active command...");
        vTaskDelay(2000 / portTICK_PERIOD_MS);
    }
    ESP_LOGI(TAG, "APP network actived.\n");

    ESP_LOGI(TAG, "writing APP network active read command...");
    while (!AppNetworkActiveReadCMD(pdp))
    {
        ESP_LOGI(TAG, "writing app network active command...");
        APPNetworkActive(pdp, ACTIVED);
        vTaskDelay(DELAY_ERROR_MSG);
        ESP_LOGI(TAG, "writing app network active read command...");
    }
}

void GNSSInit()
{
    ESP_LOGI(TAG, "writing set GNSS power mode command...");
    while (!SetGNSSPowerMode(true))
    {
        ESP_LOGI(TAG, "writing set GNSS power mode command...");
        vTaskDelay(DELAY_ERROR_MSG);
    }
    ESP_LOGI(TAG, "Power mode set.");

    ESP_LOGI(TAG, "writing set GNSS work mode command...");
    while (!SetGNSSWorkMode(gps_mode, plo_mode, bd_mode, gal_mode, qzss_mode))
    {
        ESP_LOGI(TAG, "writing set GNSS work mode command...");
        vTaskDelay(DELAY_ERROR_MSG);
    }
    ESP_LOGI(TAG, "Work mode set.");

    // ESP_LOGI(TAG, "writing set high accuracy mode command...");
    // while (!SetHighAccuracyGNSSMode())
    // {
    //     ESP_LOGI(TAG, "writing set high accuracy mode command...");
    //     vTaskDelay(DELAY_ERROR_MSG);
    // }
    // ESP_LOGI(TAG, "High accuracy mode set.");
}

void MQTTInit()
{
    while (!SetClientID(client_id))
    {
        ESP_LOGI(TAG, "Sending client ID...");
        vTaskDelay(DELAY_ERROR_MSG);
    }
    ESP_LOGI(TAG, "Set client ID.\n");

    while (!SetBrokerURL(broker_url, broker_port))
    {
        ESP_LOGI(TAG, "Sending broker URL...");
        vTaskDelay(DELAY_ERROR_MSG);
    }
    ESP_LOGI(TAG, "Set broker URL.\n");

    while (!MQTTSubscribeTopic(mqtt_topic))
    {
        ESP_LOGI(TAG, "Subscrinbing MQTT topic...");
        vTaskDelay(DELAY_ERROR_MSG);
    }
    ESP_LOGI(TAG, "Topic subscribe.\n");

    while (!SetAsyncmode(async_mode))
    {
        ESP_LOGI(TAG, "Sending set asyncmode command...");
        vTaskDelay(DELAY_ERROR_MSG);
    }
    ESP_LOGI(TAG, "Asyncmode set.\n");

    while (!SetSubhex(sub_hex))
    {
        ESP_LOGI(TAG, "Sending set data type...");
        vTaskDelay(DELAY_ERROR_MSG);
    }
    ESP_LOGI(TAG, "Data type set.\n");

    while (!SetMessageDetails(details))
    {
        ESP_LOGI(TAG, "Sending message details command...");
        vTaskDelay(DELAY_ERROR_MSG);
    }
    ESP_LOGI(TAG, "Message details set.\n");

    while (!SetQOS(qos_level))
    {
        ESP_LOGI(TAG, "Sending set QOS command");
        vTaskDelay(DELAY_ERROR_MSG);
    }
    ESP_LOGI(TAG, "QOS set level.\n");

    while (!TestCMDMQTTParameters())
    {
        vTaskDelay(DELAY_ERROR_MSG);
    }

    while (!MQTTConnect())
    {
        ESP_LOGI(TAG, "Connecting to broker...");
        vTaskDelay(2000 / portTICK_PERIOD_MS);
    }
    ESP_LOGI(TAG, "Connected to broker.\n");
}

void GPRSInit()
{
    ESP_LOGI(TAG, "Sending PDP context command...");
    while (!PDPContext(cid, pdp_type, apn_vivo))
    {
        ESP_LOGI(TAG, "Sending PDP context command...");
        vTaskDelay(DELAY_ERROR_MSG);
    }
    ESP_LOGI(TAG, "PDP context set.\n");

    ESP_LOGI(TAG, "Sending get PDP context command...");
    while (!GetPDPContext())
    {
        ESP_LOGI(TAG, "Sending get PDP context command...");
        vTaskDelay(DELAY_ERROR_MSG);
    }

    ESP_LOGI(TAG, "Sending GPRS attachment command...");
    while (!GPRSAttachment(true))
    {
        ESP_LOGI(TAG, "Sending GPRS attachment command...");
        vTaskDelay(DELAY_ERROR_MSG);
    }
    ESP_LOGI(TAG, "GPRS attached.\n");

    ESP_LOGI(TAG, "Sending check PS service command...");
    while (!CheckPSService())
    {
        ESP_LOGI(TAG, "Sending check PS service command...");
        vTaskDelay(DELAY_ERROR_MSG);
    }
}

double GetLatitude()
{
    int cont_comma = 0;
    char latitude[11] = {'\0'};
    for (int i = begin_msg_received + SIZE(CGNSINF) + 2, j = 0; i < end_msg_received; i++)
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
    for (int i = begin_msg_received + SIZE(CGNSINF) + 2, j = 0; i < end_msg_received; i++)
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
    for (int i = begin_msg_received + SIZE(CGNSINF) + 2, j = 0; i < end_msg_received; i++)
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
    for (int i = begin_msg_received + SIZE(CGNSINF) + 2, j = 0; i < end_msg_received; i++)
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

void PrintCoord()
{
    ESP_LOGI(TAG, "Latitude:\t%f", GetLatitude());
    ESP_LOGI(TAG, "Longitude:\t%f", GetLongitude());
    ESP_LOGI(TAG, "Altitude:\t%f", GetAltitude());
    ESP_LOGI(TAG, "Satellites:\t%d", GetSatellitesInView());
}