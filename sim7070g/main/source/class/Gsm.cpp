#include "../../include/class/Gsm.h"

static const char *TAG = "GSM";

const int pdp = 0;
const char ip_type = '1'; // Internet Protocol Version 4
const char *pdp_user = "vivo";
const char *pdp_password = "vivo";

// MQTT parameters
const char *broker_url = "172.104.199.107";
const char *broker_port = "1883";
const char *mqtt_topic = "NrAAFfn/0";
const char *username = "TmDev";
const char *password = "Senha-mtw@";
const char *client_id = "NrAAFfn";

// GPRS parameters
const char *apn_vivo = "zap.vivo.com.br";
const char cid = '1';
const char *pdp_addr = "";
const char *pdp_type = "IP";

extern uint64_t connect_time;

HardwareSerial SerialAT(1);

TinyGsm modem(SerialAT);
TinyGsmClient client(modem);
PubSubClient mqtt(client);

const char *broker = "172.104.199.107";
const char apn[] = "zap.vivo.com.br";
const char *topic = "GsmClientTest";

uint32_t lastReconnectAttempt = 0;

Gsm::Gsm() { Initialize(false); }

Gsm::Gsm(bool flag) { Initialize(flag); }

Gsm::Gsm(const char sn[8], bool flag)
{
    for (int i = 0; i < 8; i++)
        serial_num[i] = sn[i];
    Initialize(flag);
}

Gsm::Gsm(const char sn[8])
{
    for (int i = 0; i < 8; i++)
        serial_num[i] = sn[i];
    Initialize(false);
}

Gsm::~Gsm()
{
    ESP_LOGI(TAG, "Deleting Gsm object...");
    PWRKEYToGnd();
    vTaskDelay(5000 / portTICK_PERIOD_MS);
}

char *Gsm::GetSerialNumber() { return serial_num; }

void Gsm::SetSerialNumber(const char sn[8])
{
    for (int i = 0; i < 8; i++)
        serial_num[i] = sn[i];
}

void Gsm::Initialize(bool flag)
{
    if (!flag)
    {
        PinsSetup();
        ESP_LOGI(TAG, "Pins Setup.");

        uart2_task_init();
    }

    PWRKEYPulse();
    ESP_LOGI(TAG, "\n\nSIM7070G Init.");
    vTaskDelay(DELAY_ERROR_MSG);

    // uint8_t count = 0;
    // while (!EchoBackOff())
    // {
    //     ESP_LOGI(TAG, "Sending echo command...");
    //     if (count == ERROR_FLAG_MAX)
    //     {
    //         this->gsm_error = true;
    //         this->mqtt_error = true;
    //         this->gps_error = true;
    //         ESP_LOGE(TAG, "\n\nEcho cmd error!");
    //         return;
    //     }
    //     count++;
    //     vTaskDelay(DELAY_ERROR_MSG);
    // }
    // ESP_LOGI(TAG, "Echo mode disabled.\n");

    this->gsm_error = false;
    this->mqtt_error = false;
    this->gps_error = false;
}

/*
bool Gsm::ErrorFlagCount(bool *flag, uint8_t *count)
{
    if (*count == ERROR_FLAG_MAX)
    {
        *flag = true;
        return true;
    }
    (*count)++;
    return false;
}

void Gsm::ErrorFlagReset(bool *flag, uint8_t *count)
{
    *flag = false;
    *count = 0;
}
//*/

void Gsm::mqttCallback(char *topic, byte *payload, unsigned int len)
{
    ESP_LOGI(TAG, "Message arrived [%s]: ", topic);
    ESP_LOGI(TAG, payload, len);
    ESP_LOGI(TAG, "\n");
}

boolean Gsm::mqttConnect()
{
    ESP_LOGI("Connecting to ");
    ESP_LOGI(broker);

    // Or, if you want to authenticate MQTT:
    boolean status = mqtt.connect("GsmClientName", username, password);

    if (status == false)
    {
        ESPLOGE(TAG, "fail");
        return false;
    }
    ESP_LOGI(TAG, " success");
    mqtt.publish(topic, "GsmClientTest started");
    mqtt.subscribe(topic);
    return mqtt.connected();
}

bool Gsm::mqtt_publish(char *topic, unsigned char *msg, size_t msg_length)
{
    MQTT_status_enum status = this->get_mqtt_status();
    if (status == OFF)
    {
        ESP_LOGW(TAG, "Disconnected, trying to connect to the MQTT broker.");
        if (MQTTConnect())
        {
            ESP_LOGW(TAG, "Connected command.");
            vTaskDelay(DELAY_ERROR_MSG);
        }
        else
            return false;
        status = this->get_mqtt_status();
        if (status != ON)
            return false;
    }
    ESP_LOGI(TAG, "Connected to the broker.");
    if (status == ERROR)
        return false;
    uint16_t size = (uint16_t)msg_length;
    uint8_t digit = 0;
    string length = "";
    while (size != 0)
    {
        digit = size - ((uint16_t)(size / 10) * 10);
        length += (char)(digit + '0');
        size = (uint16_t)(size / 10);
    }
    return SendPacket(topic, strrev((char *)length.c_str()), qos_level, retain, (const char *)msg);
}

bool Gsm::mqtt_publish(unsigned char *msg, size_t msg_length, int slot)
{
    char topic[10] = "1234567/x";
    for (int i = 0; i < strlen(serial_num); i++)
    {
        topic[i] = serial_num[i];
    }
    topic[8] = '0' + slot;
    return this->mqtt_publish(topic, msg, msg_length);
}

/*
bool Gsm::GetLocation()
{
    return GetGNSS();
}

int Gsm::GetPSWMode()
{
    ESP_LOGI(TAG, "writing get power saving mode command...");
    int count = 0;
    while (!GetPowerSavingMode())
    {
        if (count > 3)
            return -1;
        count++;
        ESP_LOGI(TAG, "writing get power saving mode command...");
        vTaskDelay(DELAY_ERROR_MSG);
    }
    int index = StrContainsSubstr(&(msg_received[begin_msg_received]), CPSMS, msg_received_size, SIZE(CPSMS));
    index += begin_msg_received + SIZE(CPSMS) + 2;
    ESP_LOGI(TAG, "PSM - msg_received[%d]: %c", index, msg_received[index]);
    return msg_received[index];
}

double Gsm::GetLatitude()
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

double Gsm::GetLongitude()
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

double Gsm::GetAltitude()
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

int Gsm::GetSatellitesInView()
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

void Gsm::PrintCoord()
{
    ESP_LOGI(TAG, "Latitude:\t%f", GetLatitude());
    ESP_LOGI(TAG, "Longitude:\t%f", GetLongitude());
    ESP_LOGI(TAG, "Altitude:\t%f", GetAltitude());
    ESP_LOGI(TAG, "Satellites:\t%d", GetSatellitesInView());
}
//*/