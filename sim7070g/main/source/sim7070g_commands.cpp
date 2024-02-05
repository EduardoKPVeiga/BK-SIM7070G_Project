#include "../include/sim7070g_commands.h"

extern char message_buff[MESSAGE_BUFF_MAX_SIZE];
extern uint16_t message_pointer_pos;

static const char *TAG = "CMD";

// =======================================================================================

void BeginCMD()
{
    message_pointer_pos = 0;
    for (int i = 0; i < SIZE(BEGIN_CMD); i++)
    {
        message_buff[i] = BEGIN_CMD[i];
        message_pointer_pos++;
    }
}

void EndCMD()
{
    int aux = message_pointer_pos;
    for (int i = 0; i < SIZE(END_CMD); i++)
    {
        message_buff[i + aux] = END_CMD[i];
        message_pointer_pos++;
    }
}

void QuotationMarks()
{
    message_buff[message_pointer_pos] = '"';
    message_pointer_pos++;
}

void ValueDelimiter()
{
    message_buff[message_pointer_pos] = VALUE_DELIMITER[0];
    message_pointer_pos++;
}

void WriteCMD()
{
    message_buff[message_pointer_pos] = WRITE_CMD[0];
    message_pointer_pos++;
}

void ReadCMD()
{
    message_buff[message_pointer_pos] = READ_CMD[0];
    message_pointer_pos++;
}

void TestCMD()
{
    message_buff[message_pointer_pos] = TEST_CMD[0];
    message_pointer_pos++;
    message_buff[message_pointer_pos] = TEST_CMD[1];
    message_pointer_pos++;
}

void EnumToCharWriteBuff(uint8_t value)
{
    if (value == 0)
        message_buff[message_pointer_pos] = '0';
    else if (value == 1)
        message_buff[message_pointer_pos] = '1';
    else if (value == 2)
        message_buff[message_pointer_pos] = '2';
    else if (value == 3)
        message_buff[message_pointer_pos] = '3';
    else if (value == 4)
        message_buff[message_pointer_pos] = '4';
    else if (value == 5)
        message_buff[message_pointer_pos] = '5';
    message_pointer_pos++;
}

void AddPDPIndex(int pdpidx)
{
    if (pdpidx == 0)
        message_buff[message_pointer_pos] = '0';
    else if (pdpidx == 1)
        message_buff[message_pointer_pos] = '1';
    else if (pdpidx == 2)
        message_buff[message_pointer_pos] = '2';
    else if (pdpidx == 3)
        message_buff[message_pointer_pos] = '3';
    message_pointer_pos++;
}

void WriteStrIntoBuff(const char *str)
{
    QuotationMarks();
    for (int i = 0; i < strlen(str); i++)
    {
        message_buff[message_pointer_pos] = str[i];
        message_pointer_pos++;
    }
    QuotationMarks();
}

void WriteCmdIntoBuff(const char *cmd, CMD_action_enum action)
{
    for (int i = 0; i < strlen(cmd); i++)
    {
        message_buff[message_pointer_pos] = cmd[i];
        message_pointer_pos++;
    }
    if (action == WRITE)
        WriteCMD();
    else if (action == READ)
        ReadCMD();
    else if (action == TEST)
        TestCMD();
}
// COMMANDS ==============================================================================
// MQTT ----------------------------------------------------------------------------------
bool MQTTConnect()
{
    // AT+SMCONN
    BeginCMD();
    WriteCmdIntoBuff(SMCONN, EXE);
    EndCMD();
    return SendCMD();
}

bool MQTTDisconnect()
{
    // AT+SMDISC
    BeginCMD();
    WriteCmdIntoBuff(SMDISC, EXE);
    EndCMD();
    return SendCMD();
}

bool MQTTStatus()
{
    // AT+SMSTATE?
    BeginCMD();
    WriteCmdIntoBuff(SMSTATE, READ);
    EndCMD();
    return SendCMD();
}

bool MQTTSubscribeTopic(const char *topic)
{
    // AT+SMCONF="TOPIC","will topic"
    BeginCMD();
    WriteCmdIntoBuff(SMCONF, WRITE);
    WriteStrIntoBuff(TOPIC);
    ValueDelimiter();
    WriteStrIntoBuff(topic);
    EndCMD();
    return SendCMD();
}

bool SetClientID(const char *id)
{
    // AT+SMCONF="CLIENTID","id"
    BeginCMD();
    WriteCmdIntoBuff(SMCONF, WRITE);
    WriteStrIntoBuff(CLIENTID);
    ValueDelimiter();
    WriteStrIntoBuff(id);
    EndCMD();
    return SendCMD();
}

bool SetBrokerURL(const char *address, const char *port)
{
    // AT+SMCONF="URL","test.mosquitto.org","1883"
    BeginCMD();
    WriteCmdIntoBuff(SMCONF, WRITE);
    WriteStrIntoBuff(URL);
    ValueDelimiter();
    // QuotationMarks();
    for (int i = 0; i < strlen(address); i++)
    {
        message_buff[message_pointer_pos] = address[i];
        message_pointer_pos++;
    }
    // QuotationMarks();
    ValueDelimiter();
    // QuotationMarks();
    for (int i = 0; i < strlen(port); i++)
    {
        message_buff[message_pointer_pos] = port[i];
        message_pointer_pos++;
    }
    // QuotationMarks();
    EndCMD();
    return SendCMD();
}

bool TestCMDMQTTParameters()
{
    char cmd[SIZE(BEGIN_CMD) + SIZE(SMCONF)];
    StrConcatenate(BEGIN_CMD, SMCONF, cmd, SIZE(BEGIN_CMD), SIZE(SMCONF));
    char test_cmd[SIZE(BEGIN_CMD) + SIZE(SMCONF) + 1];
    StrConcatenate(cmd, READ_CMD, test_cmd, SIZE(BEGIN_CMD) + SIZE(SMCONF), 1);

    message_pointer_pos = 0;
    for (int i = 0; i < SIZE(BEGIN_CMD) + SIZE(SMCONF) + 1; i++)
    {
        message_buff[i] = test_cmd[i];
        message_pointer_pos++;
    }

    EndCMD();
    return SendCMD(30);
}

bool SetAsyncmode(bool mode)
{
    // AT+SMCONF="ASYNCMODE",1
    BeginCMD();
    WriteCmdIntoBuff(SMCONF, WRITE);
    WriteStrIntoBuff(ASYNCMODE);
    ValueDelimiter();

    message_buff[message_pointer_pos] = mode ? '1' : '0';
    message_pointer_pos++;

    EndCMD();
    return SendCMD();
}

bool SetSubhex(bool data_hex)
{
    // AT+SMCONF="SUBHEX",1
    BeginCMD();
    WriteCmdIntoBuff(SMCONF, WRITE);
    WriteStrIntoBuff(SUBHEX);
    ValueDelimiter();

    message_buff[message_pointer_pos] = data_hex ? '1' : '0';
    message_pointer_pos++;

    EndCMD();
    return SendCMD();
}

bool SetRetain(bool mode)
{
    // AT+SMCONF="RETAIN",1
    BeginCMD();
    WriteCmdIntoBuff(SMCONF, WRITE);
    WriteStrIntoBuff(RETAIN);
    ValueDelimiter();

    message_buff[message_pointer_pos] = mode ? '1' : '0';
    message_pointer_pos++;

    EndCMD();
    return SendCMD();
}

bool SetMessageDetails(const char *details)
{
    // AT+SMCONF="MESSAGE","will message"
    BeginCMD();
    WriteCmdIntoBuff(SMCONF, WRITE);
    WriteStrIntoBuff(MESSAGE);
    ValueDelimiter();
    WriteStrIntoBuff(details);
    EndCMD();
    return SendCMD();
}

bool SetQOS(Qos_enum level)
{
    // AT+SMCONF="QOS",1
    BeginCMD();
    WriteCmdIntoBuff(SMCONF, WRITE);
    WriteStrIntoBuff(QOS);
    ValueDelimiter();

    if (level == QOS_0)
        message_buff[message_pointer_pos] = '0';
    else if (level == QOS_1)
        message_buff[message_pointer_pos] = '1';
    else if (level == QOS_2)
        message_buff[message_pointer_pos] = '2';
    else
        return false;
    message_pointer_pos++;

    EndCMD();
    return SendCMD();
}

bool SendPacket(const char *topic, const char *length, Qos_enum qos, bool retain, const char *msg)
{
    // AT+SMPUB=<topic>,<content length>,<qos>,<retain><CR>message is enteredQuit edit mode if message length equals to <content length>
    // MQTTConnect();
    // vTaskDelay(10 / portTICK_PERIOD_MS);

    BeginCMD();
    WriteCmdIntoBuff(SMPUB, WRITE);
    WriteStrIntoBuff(topic);
    ValueDelimiter();

    for (int i = 0; i < strlen(length); i++)
    {
        message_buff[message_pointer_pos] = length[i];
        message_pointer_pos++;
    }

    ValueDelimiter();

    if (qos == QOS_0)
        message_buff[message_pointer_pos] = '0';
    else if (qos == QOS_1)
        message_buff[message_pointer_pos] = '1';
    else if (qos == QOS_2)
        message_buff[message_pointer_pos] = '2';
    message_pointer_pos++;

    ValueDelimiter();

    message_buff[message_pointer_pos] = retain ? '1' : '0';
    message_pointer_pos++;

    EndCMD();

    ESP_LOGI(TAG, "writing send packet command...");
    if (SendCMD())
    {
        int cont = 0;
        while (!mqtt_publish_flag)
        {
            if (cont == 10)
                return false;
            vTaskDelay(100 / portTICK_PERIOD_MS);
            cont++;
        }
        if (mqtt_publish_flag)
        {
            message_pointer_pos = 0;
            for (int i = 0; i < strlen(msg); i++)
            {
                message_buff[message_pointer_pos] = msg[i];
                message_pointer_pos++;
            }
            mqtt_publish_flag = false;
            return SendCMD();
        }
    }
    return false;
}

bool TestSendPacket()
{
    // AT+SMPUB=?
    BeginCMD();
    WriteCmdIntoBuff(SMPUB, TEST);
    EndCMD();
    return SendCMD();
}

bool SubscribePacket(const char *topic, Qos_enum qos)
{
    // AT+SMSUB="information",1
    BeginCMD();
    WriteCmdIntoBuff(SMSUB, WRITE);
    WriteStrIntoBuff(topic);
    ValueDelimiter();

    if (qos == QOS_0)
        message_buff[message_pointer_pos] = '0';
    else if (qos == QOS_1)
        message_buff[message_pointer_pos] = '1';
    else
        message_buff[message_pointer_pos] = '2';
    message_pointer_pos++;

    EndCMD();
    return SendCMD();
}
// ---------------------------------------------------------------------------------------

bool EchoBackOff()
{
    message_pointer_pos = 0;
    for (; message_pointer_pos < SIZE(ECHO_OFF); message_pointer_pos++)
    {
        message_buff[message_pointer_pos] = ECHO_OFF[message_pointer_pos];
    }

    EndCMD();
    return SendCMD(5);
}

// GPRS ----------------------------------------------------------------------------------
bool PingIPV4(const char *ipaddress, const char *count, const char *size, const char *timeout)
{
    // AT+SNPING4=<URL>,<count>,<size>,<timeout>
    BeginCMD();
    WriteCmdIntoBuff(SNPING4, WRITE);
    WriteStrIntoBuff(ipaddress);
    ValueDelimiter();
    WriteStrIntoBuff(count);
    ValueDelimiter();
    WriteStrIntoBuff(size);
    ValueDelimiter();
    WriteStrIntoBuff(timeout);
    EndCMD();
    return SendCMD();
}

bool GPRSAttachment(bool active)
{
    BeginCMD();
    WriteCmdIntoBuff(CGATT, WRITE);

    message_buff[message_pointer_pos] = active ? '1' : '0';
    message_pointer_pos++;

    EndCMD();
    return SendCMD(75);
}

bool PDPContext(const char cid, const char *pdp_type, const char *apn, const char *pdp_addr, D_comp_enum d_comp, H_comp_enum h_comp, bool ipv4_ctrl, bool emergency_flag)
{
    // AT+CGDCONT=<cid>[,<PDP_type>[,<APN>[,<PDP_addr>[,<d_comp>[,<h_comp>][,<ipv4_ctrl>[,<emergency_flag>]]]]]]
    BeginCMD();
    WriteCmdIntoBuff(CGDCONT, WRITE);

    message_buff[message_pointer_pos] = cid;
    message_pointer_pos++;

    ValueDelimiter();
    WriteStrIntoBuff(pdp_type);
    ValueDelimiter();
    WriteStrIntoBuff(apn);
    ValueDelimiter();
    WriteStrIntoBuff(pdp_addr);
    ValueDelimiter();
    EnumToCharWriteBuff((uint8_t)d_comp);
    ValueDelimiter();
    EnumToCharWriteBuff((uint8_t)h_comp);
    ValueDelimiter();
    EnumToCharWriteBuff((uint8_t)ipv4_ctrl);
    ValueDelimiter();
    EnumToCharWriteBuff((uint8_t)emergency_flag);
    EndCMD();
    return SendCMD();
}

bool PDPContext(const char cid, const char *pdp_type, const char *apn)
{
    // AT+CGDCONT=<cid>[,<PDP_type>[,<APN>[,<PDP_addr>[,<d_comp>[,<h_comp>][,<ipv4_ctrl>[,<emergency_flag>]]]]]]
    BeginCMD();
    WriteCmdIntoBuff(CGDCONT, WRITE);

    message_buff[message_pointer_pos] = cid;
    message_pointer_pos++;

    ValueDelimiter();
    WriteStrIntoBuff(pdp_type);
    ValueDelimiter();
    WriteStrIntoBuff(apn);
    EndCMD();
    return SendCMD();
}

bool GetPDPContext()
{
    BeginCMD();
    WriteCmdIntoBuff(CGDCONT, READ);
    EndCMD();
    return SendCMD();
}
// ---------------------------------------------------------------------------------------

// GNSS ----------------------------------------------------------------------------------
bool GetGNSS()
{
    BeginCMD();
    WriteCmdIntoBuff(CGNSINF, EXE);
    EndCMD();
    return SendCMD();
}

bool SetGNSSPowerMode(bool state)
{
    BeginCMD();
    WriteCmdIntoBuff(CGNSPWR, WRITE);

    message_buff[message_pointer_pos] = state ? '1' : '0';
    message_pointer_pos++;

    EndCMD();
    return SendCMD();
}

bool SetGNSSWorkMode(bool gps_mode, bool plo_mode, bool bd_mode, bool gal_mode, bool qzss_mode)
{
    BeginCMD();
    WriteCmdIntoBuff(CGNSMOD, WRITE);

    message_buff[message_pointer_pos] = gps_mode ? '1' : '0';
    message_pointer_pos++;

    ValueDelimiter();

    message_buff[message_pointer_pos] = plo_mode ? '1' : '0';
    message_pointer_pos++;

    ValueDelimiter();

    message_buff[message_pointer_pos] = bd_mode ? '1' : '0';
    message_pointer_pos++;

    ValueDelimiter();

    message_buff[message_pointer_pos] = gal_mode ? '1' : '0';
    message_pointer_pos++;

    ValueDelimiter();

    message_buff[message_pointer_pos] = qzss_mode ? '1' : '0';
    message_pointer_pos++;

    EndCMD();
    return SendCMD();
}

bool SetHighAccuracyGNSSMode(const char *minInterval, const char *minDistance)
{
    BeginCMD();
    WriteCmdIntoBuff(SGNSCMD, WRITE);

    message_buff[message_pointer_pos] = '2';
    message_pointer_pos++;

    ValueDelimiter();

    for (int i = 0; i < strlen(minInterval); i++)
    {
        message_buff[message_pointer_pos] = minInterval[i];
        message_pointer_pos++;
    }

    ValueDelimiter();

    for (int i = 0; i < strlen(minDistance); i++)
    {
        message_buff[message_pointer_pos] = minDistance[i];
        message_pointer_pos++;
    }

    ValueDelimiter();

    message_buff[message_pointer_pos] = '3';
    message_pointer_pos++;

    EndCMD();
    return SendCMD();
}
// ---------------------------------------------------------------------------------------

// SIMCom --------------------------------------------------------------------------------
bool ShowNetworkSystemMode()
{
    // AT+CNSMOD?
    BeginCMD();
    WriteCmdIntoBuff(CNSMOD, READ);
    EndCMD();
    return SendCMD();
}

bool GetNetworkAPN()
{
    // AT+CGNAPN
    BeginCMD();
    WriteCmdIntoBuff(CGNAPN, EXE);
    EndCMD();
    return SendCMD();
}

bool GetEPSNetworkStatus()
{
    // AT+CEREG?
    BeginCMD();
    WriteCmdIntoBuff(CEREG, READ);
    EndCMD();
    return SendCMD();
}

bool SetEPSNetworkStatus(char n)
{
    // AT+CEREG=<n>
    BeginCMD();
    WriteCmdIntoBuff(CEREG, WRITE);

    message_buff[message_pointer_pos] = n;
    message_pointer_pos++;

    EndCMD();
    return SendCMD();
}

bool GetTCPUDPConnectionStatus()
{
    // AT+CAOPEN?
    BeginCMD();
    WriteCmdIntoBuff(CAOPEN, READ);
    EndCMD();
    return SendCMD();
}

bool CheckSIMCard()
{
    // AT+CPIN?
    BeginCMD();
    WriteCmdIntoBuff(CPIN, READ);
    EndCMD();
    return SendCMD();
}

bool CheckRF()
{
    // AT+CSQ
    BeginCMD();
    WriteCmdIntoBuff(CSQ, EXE);

    EndCMD();
    if (SendCMD())
    {
        if (StrContainsSubstr(msg_received, "99", MSG_RECEIVED_BUFF_SIZE, strlen("99")))
        {
            ESP_LOGE(TAG, "<rssi> or <ber> parameters not knowm or not detectable.");
        }
        return true;
    }
    return false;
}

bool CheckPSService()
{
    // AT+CGATT?
    BeginCMD();
    WriteCmdIntoBuff(CGATT, READ);
    EndCMD();
    return SendCMD();
}

bool QueryNetworkInfo()
{
    // AT+COPS?
    BeginCMD();
    WriteCmdIntoBuff(COPS, READ);
    EndCMD();
    return SendCMD();
}

bool PDPConfigure(int pdpidx, const char ip_type, const char *apn)
{
    // AT+CNCFG=0,1,"ctnb"
    BeginCMD();
    WriteCmdIntoBuff(CNCFG, WRITE);
    AddPDPIndex(pdpidx);
    ValueDelimiter();

    message_buff[message_pointer_pos] = ip_type;
    message_pointer_pos++;

    ValueDelimiter();
    WriteStrIntoBuff(apn);
    EndCMD();
    return SendCMD();
}

bool PDPConfigureReadCMD()
{
    // AT+CNCFG?
    BeginCMD();
    WriteCmdIntoBuff(CNCFG, READ);
    EndCMD();
    return SendCMD();
}

bool BandTest()
{
    // AT+CBANDCFG?
    BeginCMD();
    WriteCmdIntoBuff(CBANDCFG, READ);
    EndCMD();
    return SendCMD();
}

bool SetNetworkType(Network_select_enum mode)
{
    // AT+CMNB=<mode>
    BeginCMD();
    WriteCmdIntoBuff(CMNB, READ);

    if (mode == CAT_M)
        message_buff[message_pointer_pos] = '1';
    else if (mode == NB_IOT)
        message_buff[message_pointer_pos] = '2';
    else if (mode == BOTH)
        message_buff[message_pointer_pos] = '3';
    message_pointer_pos++;

    EndCMD();
    return SendCMD();
}

bool GetBandScanConfig()
{
    // AT+CNBS?
    BeginCMD();
    WriteCmdIntoBuff(CNBS, READ);
    EndCMD();
    return SendCMD();
}

bool GetEngineeringModeInfo()
{
    // AT+CENG?
    BeginCMD();
    WriteCmdIntoBuff(CENG, READ);
    EndCMD();
    return SendCMD();
}
// ---------------------------------------------------------------------------------------

// NTP App -------------------------------------------------------------------------------
bool GetSynchronizeUTCTime()
{
    // AT+CNTP?
    BeginCMD();
    WriteCmdIntoBuff(CNTP, READ);
    EndCMD();
    return SendCMD();
}
// ---------------------------------------------------------------------------------------

// IP App --------------------------------------------------------------------------------
bool APPNetworkActive(int pdpidx, Action_enum action)
{
    // AT+CNACT=<pdpidx>,<action>
    BeginCMD();
    WriteCmdIntoBuff(CNACT, WRITE);
    AddPDPIndex(pdpidx);
    ValueDelimiter();

    if (action == DEACTIVED)
        message_buff[message_pointer_pos] = '0';
    else if (action == ACTIVED)
        message_buff[message_pointer_pos] = '1';
    else if (action == AUTO_ACTIVE)
        message_buff[message_pointer_pos] = '2';
    message_pointer_pos++;

    EndCMD();
    return SendCMD();
}

bool AppNetworkActiveReadCMD(int pdpidx)
{
    // AT+CNACT?
    BeginCMD();
    WriteCmdIntoBuff(CNACT, READ);
    EndCMD();
    if (SendCMD())
    {
        char pdp = '\0';
        if (pdpidx == 0)
            pdp = '0';
        else if (pdpidx == 1)
            pdp = '1';
        else if (pdpidx == 2)
            pdp = '2';
        else if (pdpidx == 3)
            pdp = '3';

        for (int i = begin_msg_received; i < end_msg_received; i++)
        {
            if (msg_received[i] == ':')
            {
                if (msg_received[i + 2] == pdp)
                {
                    if (msg_received[i + 2 + 2] == '0')
                        return false;
                    else if (msg_received[i + 2 + 2] == '1')
                        return true;
                    else
                        return false;
                }
            }
        }
    }
    return false;
}
// ---------------------------------------------------------------------------------------

// AT cmd (3GPP TS 27.00) ----------------------------------------------------------------
bool SetPhoneFunc(Cfun_enum fun)
{
    // AT+CFUN=<fun>[,<rst>]
    BeginCMD();
    WriteCmdIntoBuff(CFUN, WRITE);

    if (fun == MIN_FUNC)
        message_buff[message_pointer_pos] = '0';
    else if (fun == FULL_FUNC)
        message_buff[message_pointer_pos] = '1';
    message_pointer_pos++;

    EndCMD();
    if (SendCMD(15))
    {
        if (StrContainsSubstr(msg_received, CPIN, MSG_RECEIVED_BUFF_SIZE, SIZE(CPIN)) && StrContainsSubstr(msg_received, "READY", MSG_RECEIVED_BUFF_SIZE, strlen("READY")))
        {
            // if (StrContainsSubstr(msg_received, "NOT", MSG_RECEIVED_BUFF_SIZE, strlen("NOT")))
            // {
            //     ESP_LOGW(TAG, );
            // }
            return true;
        }
        return true; // <-----------
    }
    return false;
}
// ---------------------------------------------------------------------------------------

// AT cmd for LBS app --------------------------------------------------------------------
bool GetGSMLocation(int type, const char cid)
{
    // AT+CLBS=<type>,<cid>,[[<longitude>,<latitude>],[<lon_type>]]
    BeginCMD();
    WriteCmdIntoBuff(CLBS, WRITE);

    if (type == 1)
        message_buff[message_pointer_pos] = '1';
    else if (type == 4)
        message_buff[message_pointer_pos] = '4';
    else
        return false;
    message_pointer_pos++;

    ValueDelimiter();

    message_buff[message_pointer_pos] = cid;
    message_pointer_pos++;

    EndCMD();
    return SendCMD();
}
// ---------------------------------------------------------------------------------------