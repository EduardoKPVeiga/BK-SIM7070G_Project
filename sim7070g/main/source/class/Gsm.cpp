#include "../../include/class/Gsm.h"

static const char *TAG = "GSM";

const int pdp = 0;
const char ip_type = '1'; // Internet Protocol Version 4
const char *pdp_user = "vivo";
const char *pdp_password = "vivo";

const char *conn_type = "TCP";

const bool local_timestamp = true;

// MQTT parameters
const char *broker_url = "172.104.199.107";
const char *broker_port = "1883";
const char *mqtt_topic = "NrAAFfn/0";
const char *username = "TmDev";
const char *password = "Senha-mtw@";
const char *client_id = "NrAAFfn";
const char *details = "GPS data";
const bool async_mode = true;
const bool sub_hex = false;
const Qos_enum qos_level = QOS_0;
const bool retain = false;
const uint16_t keeptime = 65535;
const bool cleanss = true;
const Thread_action_enum action_th = WRITE_TH;
const uint16_t time_th = 10;

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

extern uint64_t connect_time;

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

    BeginCMD();
    SendCMD(1);

    uint8_t count = 0;
    while (!EchoBackOff())
    {
        ESP_LOGI(TAG, "Sending echo command...");
        if (count == ERROR_FLAG_MAX)
        {
            this->gsm_error = true;
            this->mqtt_error = true;
            this->gps_error = true;
            ESP_LOGE(TAG, "\n\nEcho cmd error!");
            return;
        }
        count++;
        vTaskDelay(DELAY_ERROR_MSG);
    }
    ESP_LOGI(TAG, "Echo mode disabled.\n");

    count = 0;
    while (!GetLocalTimeStamp(local_timestamp))
    {
        ESP_LOGI(TAG, "Sending get local timestamp command...");
        if (count == ERROR_FLAG_MAX)
        {
            this->gsm_error = true;
            this->mqtt_error = true;
            this->gps_error = true;
            ESP_LOGE(TAG, "\n\nGet time stamp cmd error!");
            return;
        }
        count++;
        vTaskDelay(DELAY_ERROR_MSG);
    }
    ESP_LOGI(TAG, "Get local time stamp mode.\n");

    count = 0;
    while (!VBATCheckingFeature(true, WRITE))
    {
        ESP_LOGI(TAG, "Sending get local timestamp command...");
        if (count == ERROR_FLAG_MAX)
        {
            this->gsm_error = true;
            this->mqtt_error = true;
            this->gps_error = true;
            ESP_LOGE(TAG, "\n\nGet time stamp cmd error!");
            return;
        }
        count++;
        vTaskDelay(DELAY_ERROR_MSG);
    }
    ESP_LOGI(TAG, "Get local time stamp mode.\n");

    this->gsm_error = false;
    this->mqtt_error = false;
    this->gps_error = false;

    if (!PDNManualActivation())
        return;
    if (!MQTTInit())
        return;
    if (!GNSSInit())
        return;
    // GPRSInit();
}

bool Gsm::PDNManualActivation()
{
    uint8_t error_cont = 0;

    ESP_LOGI(TAG, "Sending GPRS attachment command...");
    while (!GPRSAttachment(true))
    {
        ESP_LOGI(TAG, "Sending GPRS attachment command...");
        vTaskDelay(DELAY_ERROR_MSG);
    }
    ESP_LOGI(TAG, "GPRS attached.\n");

    // Disable RF
    ESP_LOGI(TAG, "writing set phone functionality command...");
    while (!SetPhoneFunc(MIN_FUNC))
    {
        ESP_LOGI(TAG, "writing set phone functionality command...");
        if (this->ErrorFlagCount(&(this->gsm_error), &error_cont))
            return false;
        vTaskDelay(DELAY_ERROR_MSG);
    }
    ESP_LOGI(TAG, "Set phone.\n");
    this->ErrorFlagReset(&(this->gsm_error), &error_cont);

    // Set the APN manually. Some operators needtoset APN first when registering the network.
    ESP_LOGI(TAG, "writing set PDP context command...");
    while (!PDPContext(cid, pdp_type, apn_vivo))
    {
        ESP_LOGI(TAG, "writing set PDP context command...");
        if (this->ErrorFlagCount(&(this->gsm_error), &error_cont))
            return false;
        vTaskDelay(DELAY_ERROR_MSG);
    }
    ESP_LOGI(TAG, "Set PDP context.\n");
    this->ErrorFlagReset(&(this->gsm_error), &error_cont);

    ESP_LOGI(TAG, "Sending GPRS attachment command...");
    while (!GPRSAttachment(true))
    {
        ESP_LOGI(TAG, "Sending GPRS attachment command...");
        vTaskDelay(DELAY_ERROR_MSG);
    }
    ESP_LOGI(TAG, "GPRS attached.\n");

    /*
        // Check PS service. 1 indicates PS has attached.
        ESP_LOGI(TAG, "writing check PS service command...");
        while (!CheckPSService())
        {
            ESP_LOGI(TAG, "Sending check PS service command...");
            if (this->ErrorFlagCount(&(this->gsm_error), &error_cont))
                return false;
            vTaskDelay(DELAY_ERROR_MSG);
        }
        this->ErrorFlagReset(&(this->gsm_error), &error_cont);
    */

    // Query the APN delivered by the network after theCAT-M or NB-IOT network is successfullyregistered
    ESP_LOGI(TAG, "writing get nertwork APN command...");
    while (!GetNetworkAPN())
    {
        ESP_LOGI(TAG, "Sending get nertwork APN command...");
        if (this->ErrorFlagCount(&(this->gsm_error), &error_cont))
            return false;
        vTaskDelay(DELAY_ERROR_MSG);
    }
    this->ErrorFlagReset(&(this->gsm_error), &error_cont);

    // Before activation please use AT+CNCFG to set APN\user name\password if needed.
    ESP_LOGI(TAG, "writing PDP configure command...");
    while (!PDPConfigure(pdp, ip_type, apn_vivo, pdp_user, pdp_password))
    {
        ESP_LOGI(TAG, "writing PDP configure command...");
        if (this->ErrorFlagCount(&(this->gsm_error), &error_cont))
            return false;
        vTaskDelay(DELAY_ERROR_MSG);
    }
    PDPConfigureReadCMD();
    ESP_LOGI(TAG, "PDP configured.\n");
    this->ErrorFlagReset(&(this->gsm_error), &error_cont);

    // Enable RF
    ESP_LOGI(TAG, "writing set phone functionality command...");
    while (!SetPhoneFunc(FULL_FUNC))
    {
        ESP_LOGI(TAG, "writing set phone functionality command...");
        if (this->ErrorFlagCount(&(this->gsm_error), &error_cont))
            return false;
        vTaskDelay(DELAY_ERROR_MSG);
    }
    ESP_LOGI(TAG, "Set phone.\n");
    this->ErrorFlagReset(&(this->gsm_error), &error_cont);

    /*
        ESP_LOGI(TAG, "writing get band scan configuration command...");
        while (!GetBandScanConfig())
        {
            ESP_LOGI(TAG, "writing get band scan configuration command...");
            if (this->ErrorFlagCount(&(this->gsm_error), &error_cont))
                return false;
            vTaskDelay(DELAY_ERROR_MSG);
        }
        this->ErrorFlagReset(&(this->gsm_error), &error_cont);

        ESP_LOGI(TAG, "writing get engineering mode information command...");
        while (!GetEngineeringModeInfo())
        {
            ESP_LOGI(TAG, "writing get engineering mode information command...");
            if (this->ErrorFlagCount(&(this->gsm_error), &error_cont))
                return false;
            vTaskDelay(DELAY_ERROR_MSG);
        }
        this->ErrorFlagReset(&(this->gsm_error), &error_cont);
    */

    // Activate network, Activate 0th PDP.
    ESP_LOGI(TAG, "writing APP network active command...");
    while (!APPNetworkActive(pdp, ACTIVED))
    {
        ESP_LOGI(TAG, "writing APP network active command...");
        if (this->ErrorFlagCount(&(this->gsm_error), &error_cont))
            return false;
        vTaskDelay(DELAY_ERROR_MSG);
    }
    ESP_LOGI(TAG, "APP network actived.\n");
    this->ErrorFlagReset(&(this->gsm_error), &error_cont);
    vTaskDelay(100 * DELAY_ERROR_MSG);

    ESP_LOGI(TAG, "writing APP network active read command...");
    while (!AppNetworkActiveReadCMD(pdp))
    {
        ESP_LOGI(TAG, "writing app network active command...");
        APPNetworkActive(pdp, ACTIVED);
        vTaskDelay(10 * DELAY_ERROR_MSG);
        ESP_LOGI(TAG, "writing app network active read command...");
        if (this->ErrorFlagCount(&(this->gsm_error), &error_cont))
            return false;
    }
    this->ErrorFlagReset(&(this->gsm_error), &error_cont);
    return true;
}

bool Gsm::MQTTInit()
{
    uint8_t error_cont = 0;

    while (!SetMqttThreadSleepTime(action_th, time_th))
    {
        ESP_LOGI(TAG, "writing set MQTT thread sleep time...");
        if (this->ErrorFlagCount(&(this->mqtt_error), &error_cont))
            return false;
        vTaskDelay(DELAY_ERROR_MSG);
    }
    ESP_LOGI(TAG, "Set mqtt thread sleep.\n");
    this->ErrorFlagReset(&(this->mqtt_error), &error_cont);

    while (!SetClientID(client_id))
    {
        ESP_LOGI(TAG, "writing client ID...");
        if (this->ErrorFlagCount(&(this->mqtt_error), &error_cont))
            return false;
        vTaskDelay(DELAY_ERROR_MSG);
    }
    ESP_LOGI(TAG, "Set client ID.\n");
    this->ErrorFlagReset(&(this->mqtt_error), &error_cont);

    while (!SetUsername(username))
    {
        ESP_LOGI(TAG, "writing user name...");
        if (this->ErrorFlagCount(&(this->mqtt_error), &error_cont))
            return false;
        vTaskDelay(DELAY_ERROR_MSG);
    }
    ESP_LOGI(TAG, "Set user name.\n");
    this->ErrorFlagReset(&(this->mqtt_error), &error_cont);

    while (!SetPassword(password))
    {
        ESP_LOGI(TAG, "writing password...");
        if (this->ErrorFlagCount(&(this->mqtt_error), &error_cont))
            return false;
        vTaskDelay(DELAY_ERROR_MSG);
    }
    ESP_LOGI(TAG, "Set password.\n");
    this->ErrorFlagReset(&(this->mqtt_error), &error_cont);

    while (!SetBrokerURL(broker_url, broker_port))
    {
        ESP_LOGI(TAG, "writing broker URL...");
        if (this->ErrorFlagCount(&(this->mqtt_error), &error_cont))
            return false;
        vTaskDelay(DELAY_ERROR_MSG);
    }
    ESP_LOGI(TAG, "Set broker URL.\n");
    this->ErrorFlagReset(&(this->mqtt_error), &error_cont);

    while (!MQTTSubscribeTopic(mqtt_topic))
    {
        ESP_LOGI(TAG, "Subscrinbing MQTT topic...");
        if (this->ErrorFlagCount(&(this->mqtt_error), &error_cont))
            return false;
        vTaskDelay(DELAY_ERROR_MSG);
    }
    ESP_LOGI(TAG, "Topic subscribe.\n");
    this->ErrorFlagReset(&(this->mqtt_error), &error_cont);

    while (!SetAsyncmode(async_mode))
    {
        ESP_LOGI(TAG, "writing set asyncmode command...");
        if (this->ErrorFlagCount(&(this->mqtt_error), &error_cont))
            return false;
        vTaskDelay(DELAY_ERROR_MSG);
    }
    ESP_LOGI(TAG, "Asyncmode set.\n");
    this->ErrorFlagReset(&(this->mqtt_error), &error_cont);

    while (!SetSubhex(sub_hex))
    {
        ESP_LOGI(TAG, "writing set data type...");
        if (this->ErrorFlagCount(&(this->mqtt_error), &error_cont))
            return false;
        vTaskDelay(DELAY_ERROR_MSG);
    }
    ESP_LOGI(TAG, "Data type set.\n");
    this->ErrorFlagReset(&(this->mqtt_error), &error_cont);

    while (!SetCleanss(cleanss))
    {
        ESP_LOGI(TAG, "writing set cleanss...");
        if (this->ErrorFlagCount(&(this->mqtt_error), &error_cont))
            return false;
        vTaskDelay(DELAY_ERROR_MSG);
    }
    ESP_LOGI(TAG, "Cleanss set.\n");
    this->ErrorFlagReset(&(this->mqtt_error), &error_cont);

    while (!SetMessageDetails(details))
    {
        ESP_LOGI(TAG, "writing message details command...");
        if (this->ErrorFlagCount(&(this->mqtt_error), &error_cont))
            return false;
        vTaskDelay(DELAY_ERROR_MSG);
    }
    ESP_LOGI(TAG, "Message details set.\n");
    this->ErrorFlagReset(&(this->mqtt_error), &error_cont);

    while (!SetQOS(qos_level))
    {
        ESP_LOGI(TAG, "writing set QOS command");
        if (this->ErrorFlagCount(&(this->mqtt_error), &error_cont))
            return false;
        vTaskDelay(DELAY_ERROR_MSG);
    }
    ESP_LOGI(TAG, "QOS set level.\n");
    this->ErrorFlagReset(&(this->mqtt_error), &error_cont);

    while (!SetKeeptime(keeptime))
    {
        ESP_LOGI(TAG, "writing set KEEPTIME command...");
        if (this->ErrorFlagCount(&(this->mqtt_error), &error_cont))
            return false;
        vTaskDelay(DELAY_ERROR_MSG);
    }
    ESP_LOGI(TAG, "KEEPTIME set time.\n");
    this->ErrorFlagReset(&(this->mqtt_error), &error_cont);

    while (!TestCMDMQTTParameters())
    {
        if (this->ErrorFlagCount(&(this->mqtt_error), &error_cont))
            return false;
        vTaskDelay(DELAY_ERROR_MSG);
    }
    this->ErrorFlagReset(&(this->mqtt_error), &error_cont);

    ESP_LOGI(TAG, "Establishing connection...");
    while (!OpenTCPconnection(cid, pdp, conn_type, broker_url, broker_port))
    {
        ESP_LOGI(TAG, "Establishing connection...");
        if (this->ErrorFlagCount(&(this->mqtt_error), &error_cont))
            return false;
        vTaskDelay(DELAY_ERROR_MSG);
    }
    ESP_LOGI(TAG, "Connected.\n");
    this->ErrorFlagReset(&(this->mqtt_error), &error_cont);

    while (!MQTTConnect())
    {
        ESP_LOGI(TAG, "Connecting to broker...");
        if (this->ErrorFlagCount(&(this->mqtt_error), &error_cont))
            return false;
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
    connect_time = esp_timer_get_time();
    ESP_LOGI(TAG, "Connected to broker.\n");
    this->ErrorFlagReset(&(this->mqtt_error), &error_cont);
    return true;

    while (!SubscribePacket(mqtt_topic, qos_level))
    {
        ESP_LOGI(TAG, "writing subscribe command...");
        if (this->ErrorFlagCount(&(this->mqtt_error), &error_cont))
            return false;
        vTaskDelay(DELAY_ERROR_MSG);
    }
    ESP_LOGI(TAG, "Subscribe.\n");
    this->ErrorFlagReset(&(this->mqtt_error), &error_cont);
    return true;
}

bool Gsm::GNSSInit()
{
    uint8_t error_cont = 0;
    ESP_LOGI(TAG, "writing set GNSS power mode command...");
    while (!SetGNSSPowerMode(true))
    {
        ESP_LOGI(TAG, "writing set GNSS power mode command...");
        if (this->ErrorFlagCount(&(this->gps_error), &error_cont))
            return false;
        vTaskDelay(DELAY_ERROR_MSG);
    }
    ESP_LOGI(TAG, "Power mode set.");
    this->ErrorFlagReset(&(this->gps_error), &error_cont);

    ESP_LOGI(TAG, "writing set GNSS work mode command...");
    while (!SetGNSSWorkMode(gps_mode, plo_mode, bd_mode, gal_mode, qzss_mode))
    {
        ESP_LOGI(TAG, "writing set GNSS work mode command...");
        if (this->ErrorFlagCount(&(this->gps_error), &error_cont))
            return false;
        vTaskDelay(DELAY_ERROR_MSG);
    }
    ESP_LOGI(TAG, "Work mode set.");
    this->ErrorFlagReset(&(this->gps_error), &error_cont);

    // ESP_LOGI(TAG, "writing set high accuracy mode command...");
    // while (!SetHighAccuracyGNSSMode())
    // {
    //     ESP_LOGI(TAG, "writing set high accuracy mode command...");
    //     vTaskDelay(DELAY_ERROR_MSG);
    // }
    // ESP_LOGI(TAG, "High accuracy mode set.");
    return true;
}

void Gsm::GPRSInit()
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

MQTT_status_enum Gsm::get_mqtt_status()
{
    MQTTStatus();
    int index = StrContainsSubstr(&(msg_received[begin_msg_received]), SMSTATE, msg_received_size, SIZE(SMSTATE));
    if (index >= 0)
    {
        index += begin_msg_received + SIZE(SMSTATE) + 2;
        if (index < MSG_RECEIVED_BUFF_SIZE)
        {
            ESP_LOGI(TAG, "msg_received[%d]: %c", index, msg_received[index]);
            if (msg_received[index] == '0')
            {
                ESP_LOGE(TAG, "Disconnect time: %d s", (int)((esp_timer_get_time() - connect_time) / 1000000));
                return OFF;
            }
            if (msg_received[index] == '1')
            {
                ESP_LOGI(TAG, "Time since connection: %d s", (int)((esp_timer_get_time() - connect_time) / 1000000));
                return ON;
            }
            if (msg_received[index] == '2')
            {
                ESP_LOGI(TAG, "Time since connection: %d s", (int)((esp_timer_get_time() - connect_time) / 1000000));
                return ON;
            }
        }
    }
    ESP_LOGE(TAG, "Error time: %d s", (int)((esp_timer_get_time() - connect_time) / 1000000));
    return ERROR;
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