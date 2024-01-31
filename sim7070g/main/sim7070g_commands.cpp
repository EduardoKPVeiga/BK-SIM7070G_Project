#include "sim7070g_commands.h"

extern char message_buff[MESSAGE_BUFF_MAX_SIZE];
extern uint16_t message_pointer_pos;

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

void OpenBracket()
{
    /*
    message_buff[message_pointer_pos] = OPEN_BRACKET[0];
    message_pointer_pos++;
    */
}

void CloseBracket()
{
    /*
    message_buff[message_pointer_pos] = CLOSE_BRACKET[0];
    message_pointer_pos++;
    */
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

// COMMANDS ==============================================================================
// MQTT ----------------------------------------------------------------------------------
bool MQTTConnect()
{
    // AT+SMCONN
    BeginCMD();

    for (int i = 0; i < SIZE(SMCONN); i++)
    {
        message_buff[message_pointer_pos] = SMCONN[i];
        message_pointer_pos++;
    }

    EndCMD();
    return SendCMD();
}

void MQTTDisconnect()
{
    // AT+SMDISC
    BeginCMD();

    for (int i = 0; i < SIZE(SMDISC); i++)
    {
        message_buff[message_pointer_pos] = SMDISC[i];
        message_pointer_pos++;
    }

    EndCMD();

    SendCMD();
}

bool MQTTSubscribeTopic(const char *topic)
{
    // AT+SMCONF="TOPIC","will topic"
    BeginCMD();

    for (int i = 0; i < SIZE(SMCONF); i++)
    {
        message_buff[message_pointer_pos] = SMCONF[i];
        message_pointer_pos++;
    }

    WriteCMD();

    QuotationMarks();
    for (int i = 0; i < SIZE(TOPIC); i++)
    {
        message_buff[message_pointer_pos] = TOPIC[i];
        message_pointer_pos++;
    }
    QuotationMarks();

    ValueDelimiter();

    QuotationMarks();
    for (int i = 0; i < strlen(topic); i++)
    {
        message_buff[message_pointer_pos] = topic[i];
        message_pointer_pos++;
    }
    QuotationMarks();

    EndCMD();

    return SendCMD();
}

bool SetClientID(const char *id)
{
    // AT+SMCONF="CLIENTID","id"
    BeginCMD();

    for (int i = 0; i < SIZE(SMCONF); i++)
    {
        message_buff[message_pointer_pos] = SMCONF[i];
        message_pointer_pos++;
    }

    WriteCMD();

    QuotationMarks();
    for (int i = 0; i < SIZE(CLIENTID); i++)
    {
        message_buff[message_pointer_pos] = CLIENTID[i];
        message_pointer_pos++;
    }
    QuotationMarks();

    ValueDelimiter();

    QuotationMarks();
    for (int i = 0; i < strlen(id); i++)
    {
        message_buff[message_pointer_pos] = id[i];
        message_pointer_pos++;
    }
    QuotationMarks();

    EndCMD();

    return SendCMD();
}

bool SetBrokerURL(const char *address, const char *port)
{
    // AT+SMCONF="URL","test.mosquitto.org","1883"
    BeginCMD();

    for (int i = 0; i < SIZE(SMCONF); i++)
    {
        message_buff[message_pointer_pos] = SMCONF[i];
        message_pointer_pos++;
    }

    WriteCMD();

    QuotationMarks();
    for (int i = 0; i < SIZE(URL); i++)
    {
        message_buff[message_pointer_pos] = URL[i];
        message_pointer_pos++;
    }
    QuotationMarks();

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

    for (int i = 0; i < SIZE(SMCONF); i++)
    {
        message_buff[message_pointer_pos] = SMCONF[i];
        message_pointer_pos++;
    }

    WriteCMD();

    QuotationMarks();
    for (int i = 0; i < SIZE(ASYNCMODE); i++)
    {
        message_buff[message_pointer_pos] = ASYNCMODE[i];
        message_pointer_pos++;
    }
    QuotationMarks();

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
    for (int i = 0; i < SIZE(SMCONF); i++)
    {
        message_buff[message_pointer_pos] = SMCONF[i];
        message_pointer_pos++;
    }

    WriteCMD();

    QuotationMarks();
    for (int i = 0; i < SIZE(SUBHEX); i++)
    {
        message_buff[message_pointer_pos] = SUBHEX[i];
        message_pointer_pos++;
    }
    QuotationMarks();

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
    for (int i = 0; i < SIZE(SMCONF); i++)
    {
        message_buff[message_pointer_pos] = SMCONF[i];
        message_pointer_pos++;
    }

    WriteCMD();

    QuotationMarks();
    for (int i = 0; i < SIZE(RETAIN); i++)
    {
        message_buff[message_pointer_pos] = RETAIN[i];
        message_pointer_pos++;
    }
    QuotationMarks();

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
    for (int i = 0; i < SIZE(SMCONF); i++)
    {
        message_buff[message_pointer_pos] = SMCONF[i];
        message_pointer_pos++;
    }

    WriteCMD();

    QuotationMarks();
    for (int i = 0; i < SIZE(MESSAGE); i++)
    {
        message_buff[message_pointer_pos] = MESSAGE[i];
        message_pointer_pos++;
    }
    QuotationMarks();

    ValueDelimiter();

    QuotationMarks();
    for (int i = 0; i < strlen(details); i++)
    {
        message_buff[message_pointer_pos] = details[i];
        message_pointer_pos++;
    }
    QuotationMarks();

    EndCMD();
    return SendCMD();
}

bool SetQOS(Qos_enum level)
{
    // AT+SMCONF="QOS",1
    BeginCMD();
    for (int i = 0; i < SIZE(SMCONF); i++)
    {
        message_buff[message_pointer_pos] = SMCONF[i];
        message_pointer_pos++;
    }

    WriteCMD();

    QuotationMarks();
    for (int i = 0; i < SIZE(QOS); i++)
    {
        message_buff[message_pointer_pos] = QOS[i];
        message_pointer_pos++;
    }
    QuotationMarks();

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
    BeginCMD();

    for (int i = 0; i < SIZE(SMPUB); i++)
    {
        message_buff[message_pointer_pos] = SMPUB[i];
        message_pointer_pos++;
    }

    WriteCMD();

    QuotationMarks();
    for (int i = 0; i < strlen(topic); i++)
    {
        message_buff[message_pointer_pos] = topic[i];
        message_pointer_pos++;
    }
    QuotationMarks();

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

    int cont = 0;
    ESP_LOGI("CMD", "writing send packet command...");
    while (!SendCMD() && cont < 5)
    {
        ESP_LOGI("CMD", "writing send packet command...");
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
    if (StrContainsSubstr(msg_received, ">", 4, 1))
    {
        message_pointer_pos = 0;
        for (int i = 0; i < strlen(msg); i++)
        {
            message_buff[message_pointer_pos] = msg[i];
            message_pointer_pos++;
        }

        return SendCMD();
    }
    return false;
}

bool TestSendPacket()
{
    BeginCMD();
    for (int i = 0; i < SIZE(SMPUB); i++)
    {
        message_buff[message_pointer_pos] = SMPUB[i];
        message_pointer_pos++;
    }
    TestCMD();
    EndCMD();
    return SendCMD();
}

bool MQTTReceiveSubscribeData()
{
    message_pointer_pos = 0;
    for (int i = 0; i < SIZE(SMSUB); i++)
    {
        message_buff[message_pointer_pos] = SMSUB[i];
        message_pointer_pos++;
    }
    EndCMD();
    return SendCMD();
}

bool SubscribePacket(const char *topic, Qos_enum qos)
{
    BeginCMD();
    for (int i = 0; i < SIZE(SMSUB); i++)
    {
        message_buff[message_pointer_pos] = SMSUB[i];
        message_pointer_pos++;
    }
    WriteCMD();
    QuotationMarks();
    for (int i = 0; i < strlen(topic); i++)
    {
        message_buff[message_pointer_pos] = topic[i];
        message_pointer_pos++;
    }
    QuotationMarks();
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
    BeginCMD();

    for (int i = 0; i < SIZE(SNPING4); i++)
    {
        message_buff[message_pointer_pos] = SNPING4[i];
        message_pointer_pos++;
    }

    WriteCMD();

    QuotationMarks();
    for (int i = 0; i < strlen(ipaddress); i++)
    {
        message_buff[message_pointer_pos] = ipaddress[i];
        message_pointer_pos++;
    }
    QuotationMarks();

    ValueDelimiter();

    QuotationMarks();
    for (int i = 0; i < strlen(count); i++)
    {
        message_buff[message_pointer_pos] = count[i];
        message_pointer_pos++;
    }
    QuotationMarks();

    ValueDelimiter();

    QuotationMarks();
    for (int i = 0; i < strlen(size); i++)
    {
        message_buff[message_pointer_pos] = size[i];
        message_pointer_pos++;
    }
    QuotationMarks();

    ValueDelimiter();

    QuotationMarks();
    for (int i = 0; i < strlen(timeout); i++)
    {
        message_buff[message_pointer_pos] = timeout[i];
        message_pointer_pos++;
    }
    QuotationMarks();

    EndCMD();

    return SendCMD();
}

bool GPRSAttachment(bool active)
{
    BeginCMD();

    for (int i = 0; i < SIZE(CGATT); i++)
    {
        message_buff[message_pointer_pos] = CGATT[i];
        message_pointer_pos++;
    }

    WriteCMD();

    message_buff[message_pointer_pos] = active ? '1' : '0';
    message_pointer_pos++;

    EndCMD();
    return SendCMD(75);
}

bool PDPContext(const char cid, const char *pdp_type, const char *apn, const char *pdp_addr, D_comp_enum d_comp, H_comp_enum h_comp, bool ipv4_ctrl, bool emergency_flag)
{
    BeginCMD();

    for (int i = 0; i < SIZE(CGDCONT); i++)
    {
        message_buff[message_pointer_pos] = CGDCONT[i];
        message_pointer_pos++;
    }

    WriteCMD();

    message_buff[message_pointer_pos] = cid;
    message_pointer_pos++;

    ValueDelimiter();

    QuotationMarks();
    for (int i = 0; i < strlen(pdp_type); i++)
    {
        message_buff[message_pointer_pos] = pdp_type[i];
        message_pointer_pos++;
    }
    QuotationMarks();

    ValueDelimiter();
    QuotationMarks();
    for (int i = 0; i < strlen(apn); i++)
    {
        message_buff[message_pointer_pos] = apn[i];
        message_pointer_pos++;
    }
    QuotationMarks();

    ValueDelimiter();
    QuotationMarks();
    for (int i = 0; i < strlen(pdp_addr); i++)
    {
        message_buff[message_pointer_pos] = pdp_addr[i];
        message_pointer_pos++;
    }
    QuotationMarks();

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
    BeginCMD();

    for (int i = 0; i < SIZE(CGDCONT); i++)
    {
        message_buff[message_pointer_pos] = CGDCONT[i];
        message_pointer_pos++;
    }

    WriteCMD();

    message_buff[message_pointer_pos] = cid;
    message_pointer_pos++;

    ValueDelimiter();

    QuotationMarks();
    for (int i = 0; i < strlen(pdp_type); i++)
    {
        message_buff[message_pointer_pos] = pdp_type[i];
        message_pointer_pos++;
    }
    QuotationMarks();

    ValueDelimiter();
    QuotationMarks();
    for (int i = 0; i < strlen(apn); i++)
    {
        message_buff[message_pointer_pos] = apn[i];
        message_pointer_pos++;
    }
    QuotationMarks();
    EndCMD();
    return SendCMD();
}

bool GetPDPContext()
{
    BeginCMD();

    for (int i = 0; i < SIZE(CGDCONT); i++)
    {
        message_buff[message_pointer_pos] = CGDCONT[i];
        message_pointer_pos++;
    }

    ReadCMD();

    EndCMD();
    return SendCMD();
}
// ---------------------------------------------------------------------------------------

// GNSS ----------------------------------------------------------------------------------
bool GetGNSS()
{
    BeginCMD();

    for (int i = 0; i < SIZE(CGNSINF); i++)
    {
        message_buff[message_pointer_pos] = CGNSINF[i];
        message_pointer_pos++;
    }

    EndCMD();
    return SendCMD();
}

bool SetGNSSPowerMode(bool state)
{
    BeginCMD();

    for (int i = 0; i < SIZE(CGNSPWR); i++)
    {
        message_buff[message_pointer_pos] = CGNSPWR[i];
        message_pointer_pos++;
    }

    WriteCMD();

    message_buff[message_pointer_pos] = state ? '1' : '0';
    message_pointer_pos++;

    EndCMD();
    return SendCMD();
}

bool SetGNSSWorkMode(bool gps_mode, bool plo_mode, bool bd_mode, bool gal_mode, bool qzss_mode)
{
    BeginCMD();

    for (int i = 0; i < SIZE(CGNSMOD); i++)
    {
        message_buff[message_pointer_pos] = CGNSMOD[i];
        message_pointer_pos++;
    }

    WriteCMD();

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
// ---------------------------------------------------------------------------------------

// SIMCom --------------------------------------------------------------------------------
bool ShowNetworkSystemMode()
{
    // AT+CNSMOD?
    BeginCMD();

    for (int i = 0; i < SIZE(CNSMOD); i++)
    {
        message_buff[message_pointer_pos] = CNSMOD[i];
        message_pointer_pos++;
    }
    ReadCMD();

    EndCMD();
    return SendCMD();
}

bool GetNetworkAPN()
{
    // AT+CGNAPN
    BeginCMD();

    for (int i = 0; i < SIZE(CGNAPN); i++)
    {
        message_buff[message_pointer_pos] = CGNAPN[i];
        message_pointer_pos++;
    }

    EndCMD();
    return SendCMD();
}

bool GetEPSNetworkStatus()
{
    // AT+CEREG?
    BeginCMD();

    for (int i = 0; i < SIZE(CEREG); i++)
    {
        message_buff[message_pointer_pos] = CEREG[i];
        message_pointer_pos++;
    }
    ReadCMD();

    EndCMD();
    return SendCMD();
}

bool SetEPSNetworkStatus(char n)
{
    // AT+CEREG=<n>
    BeginCMD();

    for (int i = 0; i < SIZE(CEREG); i++)
    {
        message_buff[message_pointer_pos] = CEREG[i];
        message_pointer_pos++;
    }
    WriteCMD();

    message_buff[message_pointer_pos] = n;
    message_pointer_pos++;

    EndCMD();
    return SendCMD();
}

bool GetTCPUDPConnectionStatus()
{
    // AT+CAOPEN?
    BeginCMD();

    for (int i = 0; i < SIZE(CAOPEN); i++)
    {
        message_buff[message_pointer_pos] = CAOPEN[i];
        message_pointer_pos++;
    }
    ReadCMD();

    EndCMD();
    return SendCMD();
}

bool CheckSIMCard()
{
    // AT+CPIN?
    BeginCMD();

    for (int i = 0; i < SIZE(CPIN); i++)
    {
        message_buff[message_pointer_pos] = CPIN[i];
        message_pointer_pos++;
    }
    ReadCMD();

    EndCMD();
    return SendCMD();
}

bool CheckRF()
{
    // AT+CSQ
    BeginCMD();

    for (int i = 0; i < SIZE(CSQ); i++)
    {
        message_buff[message_pointer_pos] = CSQ[i];
        message_pointer_pos++;
    }

    EndCMD();
    if (SendCMD())
    {
        if (StrContainsSubstr(msg_received, "99", MSG_RECEIVED_BUFF_SIZE, strlen("99")))
        {
            ESP_LOGE("CMD", "<rssi> or <ber> parameters not knowm or not detectable.");
        }
        return true;
    }
    return false;
}

bool CheckPSService()
{
    // AT+CGATT?
    BeginCMD();

    for (int i = 0; i < SIZE(CGATT); i++)
    {
        message_buff[message_pointer_pos] = CGATT[i];
        message_pointer_pos++;
    }
    ReadCMD();

    EndCMD();
    return SendCMD();
}

bool QueryNetworkInfo()
{
    // AT+COPS?
    BeginCMD();

    for (int i = 0; i < SIZE(COPS); i++)
    {
        message_buff[message_pointer_pos] = COPS[i];
        message_pointer_pos++;
    }
    ReadCMD();

    EndCMD();
    return SendCMD();
}

bool PDPConfigure(const char pdpidx, const char ip_type, const char *apn)
{
    // AT+CNCFG=0,1,"ctnb"
    BeginCMD();

    for (int i = 0; i < SIZE(CNCFG); i++)
    {
        message_buff[message_pointer_pos] = CNCFG[i];
        message_pointer_pos++;
    }
    WriteCMD();

    message_buff[message_pointer_pos] = pdpidx;
    message_pointer_pos++;

    ValueDelimiter();

    message_buff[message_pointer_pos] = ip_type;
    message_pointer_pos++;

    ValueDelimiter();

    QuotationMarks();
    for (int i = 0; i < strlen(apn); i++)
    {
        message_buff[message_pointer_pos] = apn[i];
        message_pointer_pos++;
    }
    QuotationMarks();

    EndCMD();
    return SendCMD();
}

bool PDPConfigureReadCMD()
{
    // AT+CNCFG?
    BeginCMD();
    for (int i = 0; i < SIZE(CNCFG); i++)
    {
        message_buff[message_pointer_pos] = CNCFG[i];
        message_pointer_pos++;
    }
    ReadCMD();
    EndCMD();
    return SendCMD();
}

bool BandTest()
{
    // AT+CBANDCFG?
    BeginCMD();

    for (int i = 0; i < SIZE(CBANDCFG); i++)
    {
        message_buff[message_pointer_pos] = CBANDCFG[i];
        message_pointer_pos++;
    }
    ReadCMD();

    EndCMD();
    return SendCMD();
}

bool SetNetworkType(Network_select_enum mode)
{
    // AT+CMNB=<mode>
    BeginCMD();

    for (int i = 0; i < SIZE(CMNB); i++)
    {
        message_buff[message_pointer_pos] = CMNB[i];
        message_pointer_pos++;
    }

    WriteCMD();

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
// ---------------------------------------------------------------------------------------

// NTP App -------------------------------------------------------------------------------
bool GetSynchronizeUTCTime()
{
    BeginCMD();

    for (int i = 0; i < SIZE(CNTP); i++)
    {
        message_buff[message_pointer_pos] = CNTP[i];
        message_pointer_pos++;
    }

    ReadCMD();

    EndCMD();
    return SendCMD();
}
// ---------------------------------------------------------------------------------------

// IP App --------------------------------------------------------------------------------
bool APPNetworkActive(const char pdpidx, Action_enum action)
{
    // AT+CNACT=<pdpidx>,<action>
    BeginCMD();

    for (int i = 0; i < SIZE(CNACT); i++)
    {
        message_buff[message_pointer_pos] = CNACT[i];
        message_pointer_pos++;
    }

    WriteCMD();

    message_buff[message_pointer_pos] = pdpidx;
    message_pointer_pos++;

    ValueDelimiter();

    if (action == DEACTIVED)
        message_buff[message_pointer_pos] = '0';
    else if (action == ACTIVED)
        message_buff[message_pointer_pos] = '1';
    else if (action == AUTO_ACTIVE)
        message_buff[message_pointer_pos] = '2';
    message_pointer_pos++;
    EndCMD();

    char *deactived = "DEACTIVE";
    if (SendCMD())
    {
        return (StrContainsSubstr(msg_received, deactived, MSG_RECEIVED_BUFF_SIZE, strlen("DEACTIVE"))) ? false : true;
    }
    return false;
}

bool AppNetworkActiveReadCMD()
{
    // AT+CNACT?
    BeginCMD();
    for (int i = 0; i < SIZE(CNACT); i++)
    {
        message_buff[message_pointer_pos] = CNACT[i];
        message_pointer_pos++;
    }
    ReadCMD();
    EndCMD();
    return SendCMD();
}
// ---------------------------------------------------------------------------------------

// AT cmd (3GPP TS 27.00) ----------------------------------------------------------------
bool SetPhoneFunc(Cfun_enum fun)
{
    // AT+CFUN=<fun>[,<rst>]
    BeginCMD();
    for (int i = 0; i < SIZE(CFUN); i++)
    {
        message_buff[message_pointer_pos] = CFUN[i];
        message_pointer_pos++;
    }
    WriteCMD();
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
            //     ESP_LOGW("CMD", );
            // }
            return true;
        }
        return true; // <-----------
    }
    return false;
}
// ---------------------------------------------------------------------------------------