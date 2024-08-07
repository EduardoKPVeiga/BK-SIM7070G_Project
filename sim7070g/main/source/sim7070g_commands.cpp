#include "../include/sim7070g_commands.h"

extern char message_buff[MESSAGE_BUFF_MAX_SIZE];
extern uint16_t message_pointer_pos;

static const char *TAG = "CMD";

uint64_t connect_time = 0;

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

void WriteNumberIntoBuff(uint32_t num)
{
    WriteCharArrayIntoBuff(DecimalToCharArray(num));
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

void WriteCharArrayIntoBuff(const char *a)
{
    for (int i = 0; i < strlen(a); i++)
    {
        message_buff[message_pointer_pos] = a[i];
        message_pointer_pos++;
    }
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
bool SetMqttThreadSleepTime(Thread_action_enum action, uint16_t time)
{
    BeginCMD();
    WriteCmdIntoBuff(SMRCVSLPTM, WRITE);
    WriteNumberIntoBuff((uint8_t)action);
    ValueDelimiter();
    WriteCharArrayIntoBuff(DecimalToCharArray(time));
    EndCMD();
    return SendCMD();
}

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

int MQTTStatus()
{
    // AT+SMSTATE?
    BeginCMD();
    WriteCmdIntoBuff(SMSTATE, READ);
    EndCMD();
    if (SendCMD())
    {
        int size_sub_str = StrContainsSubstr(&(msg_received[begin_msg_received]), SMSTATE, msg_received_size, SIZE(SMSTATE));
        if (size_sub_str >= 0)
        {
            int index = size_sub_str + begin_msg_received + 2 + SIZE(SMSTATE);
            return (msg_received[index] == '1');
        }
    }
    return -1;
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
    WriteNumberIntoBuff((uint8_t)level);
    EndCMD();
    return SendCMD();
}

bool SetKeeptime(uint16_t keeptime)
{
    // AT+SMCONF="KEEPTIME",60
    BeginCMD();
    WriteCmdIntoBuff(SMCONF, WRITE);
    WriteStrIntoBuff(KEEPTIME);
    ValueDelimiter();
    WriteNumberIntoBuff(keeptime);
    EndCMD();
    return SendCMD();
}

bool SetCleanss(bool cleanss)
{
    BeginCMD();
    WriteCmdIntoBuff(SMCONF, WRITE);
    WriteStrIntoBuff(CLEANSS);
    ValueDelimiter();
    WriteNumberIntoBuff((uint8_t)cleanss);
    EndCMD();
    return SendCMD();
}

bool SetUsername(const char *usname)
{
    BeginCMD();
    WriteCmdIntoBuff(SMCONF, WRITE);
    WriteStrIntoBuff(USERNAME);
    ValueDelimiter();
    WriteStrIntoBuff(usname);
    EndCMD();
    return SendCMD();
}

bool SetPassword(const char *pass)
{
    BeginCMD();
    WriteCmdIntoBuff(SMCONF, WRITE);
    WriteStrIntoBuff(PASSWORD);
    ValueDelimiter();
    WriteStrIntoBuff(pass);
    EndCMD();
    return SendCMD();
}

bool SendPacket(const char *topic, const char *length, Qos_enum qos, bool retain, const char *msg)
{
    // AT+SMPUB=<topic>,<content length>,<qos>,<retain><CR>message is enteredQuit edit mode if message length equals to <content length>
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
    WriteNumberIntoBuff((uint8_t)qos);
    ValueDelimiter();
    WriteNumberIntoBuff((uint8_t)retain);
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
            ESP_LOGW(TAG, "Time since connection: %d s", (int)((esp_timer_get_time() - connect_time) / (int)1000000));
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
    WriteNumberIntoBuff((uint8_t)qos);
    EndCMD();
    return SendCMD();
}

bool UnsubscribePacket(const char *topic)
{
    // AT+SMUNSUB=<topic>
    BeginCMD();
    WriteCmdIntoBuff(SMUNSUB, WRITE);
    WriteStrIntoBuff(topic);
    EndCMD();
    return SendCMD();
}
// ---------------------------------------------------------------------------------------

bool EchoBackOff()
{
    message_pointer_pos = 0;
    for (; message_pointer_pos < SIZE(ECHO_OFF); message_pointer_pos++)
        message_buff[message_pointer_pos] = ECHO_OFF[message_pointer_pos];
    EndCMD();
    return SendCMD(5);
}

// GPRS ----------------------------------------------------------------------------------

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
    WriteNumberIntoBuff((uint8_t)state);
    EndCMD();
    return SendCMD();
}

bool SetGNSSWorkMode(bool gps_mode, bool plo_mode, bool bd_mode, bool gal_mode, bool qzss_mode)
{
    BeginCMD();
    WriteCmdIntoBuff(CGNSMOD, WRITE);
    WriteNumberIntoBuff((uint8_t)gps_mode);
    ValueDelimiter();
    WriteNumberIntoBuff((uint8_t)plo_mode);
    ValueDelimiter();
    WriteNumberIntoBuff((uint8_t)bd_mode);
    ValueDelimiter();
    WriteNumberIntoBuff((uint8_t)gal_mode);
    ValueDelimiter();
    WriteNumberIntoBuff((uint8_t)qzss_mode);
    EndCMD();
    return SendCMD();
}

bool SetHighAccuracyGNSSMode(const char *minInterval, const char *minDistance)
{
    BeginCMD();
    WriteCmdIntoBuff(SGNSCMD, WRITE);
    WriteNumberIntoBuff(2);
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
    WriteNumberIntoBuff(3);
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

#ifdef PSM
bool GetEPSNetworkStatus()
{
    // AT+CEREG?
    BeginCMD();
    WriteCmdIntoBuff(CEREG, READ);
    EndCMD();
    return SendCMD();
}

bool SetEPSNetworkStatus(int n)
{
    // AT+CEREG=<n>
    BeginCMD();
    WriteCmdIntoBuff(CEREG, WRITE);
    WriteNumberIntoBuff(n);
    EndCMD();
    return SendCMD();
}
#endif

bool GetCSDNetworkStatus()
{
    // AT+CREG?
    BeginCMD();
    WriteCmdIntoBuff(CREG, READ);
    EndCMD();
    return SendCMD();
}

bool GetGPRSNetworkStatus()
{
    // AT+CGREG?
    BeginCMD();
    WriteCmdIntoBuff(CGREG, READ);
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
        if (StrContainsSubstr(msg_received, "99", MSG_RECEIVED_BUFF_SIZE, strlen("99")) >= 0)
            ESP_LOGE(TAG, "<rssi> or <ber> parameters not knowm or not detectable.");
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

bool PDPConfigure(int pdpidx, const char ip_type, const char *apn, const char *user, const char *password)
{
    // AT+CNCFG=0,1,"ctnb"
    BeginCMD();
    WriteCmdIntoBuff(CNCFG, WRITE);
    WriteNumberIntoBuff(0);
    ValueDelimiter();
    message_buff[message_pointer_pos] = ip_type;
    message_pointer_pos++;
    ValueDelimiter();
    WriteStrIntoBuff(apn);
    ValueDelimiter();
    WriteStrIntoBuff(user);
    ValueDelimiter();
    WriteStrIntoBuff(password);
    ValueDelimiter();
    WriteNumberIntoBuff(0);
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
    WriteNumberIntoBuff((uint8_t)mode);
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

bool SetSlowClockMode(bool mode)
{
    BeginCMD();
    WriteCmdIntoBuff(CSCLK, WRITE);
    WriteNumberIntoBuff((uint8_t)mode);
    EndCMD();
    return SendCMD(60);
}

int GetSlowClockMode()
{
    // AT+CSCLK?
    BeginCMD();
    WriteCmdIntoBuff(CSCLK, READ);
    EndCMD();
    if (SendCMD())
    {
        bool find = false;
        int size_sub_str = -1;
        int index = 0;
        while (!find)
        {
            if (index + SIZE(CSCLK) > end_msg_received)
                return -1;
            size_sub_str = StrContainsSubstr(&(msg_received[begin_msg_received + index]), CSCLK, msg_received_size, SIZE(CSCLK));
            if (size_sub_str >= 0)
            {
                index += size_sub_str + 2 + SIZE(CSCLK);
                if (msg_received[begin_msg_received + index - 2] == ':')
                    find = true;
                else
                    index -= 2;
            }
        }
        return (msg_received[begin_msg_received + index] == '1');
    }
    return -1;
}

#ifdef PSM
bool SetPowerSavingMode(bool mode)
{
    BeginCMD();
    WriteCmdIntoBuff(CPSMS, WRITE);
    WriteNumberIntoBuff((uint8_t)mode);
    if (mode)
    {
        ValueDelimiter();
        ValueDelimiter();
        ValueDelimiter();
        QuotationMarks();
        WriteCharArrayIntoBuff(BintoCharArray(PSM_WAKE_UP_TIMER));
        QuotationMarks();
        ValueDelimiter();
        QuotationMarks();
        WriteCharArrayIntoBuff(BintoCharArray(REQ_PERIODIC_TAU));
        QuotationMarks();
    }
    EndCMD();
    return SendCMD(60);
}

bool GetPowerSavingMode()
{
    BeginCMD();
    WriteCmdIntoBuff(CPSMS, READ);
    EndCMD();
    return SendCMD();
}
#endif

bool GetLocalTimeStamp(bool mode)
{
    BeginCMD();
    WriteCmdIntoBuff(CLTS, WRITE);
    WriteNumberIntoBuff((uint8_t)mode);
    EndCMD();
    return SendCMD();
}

bool VBATCheckingFeature(bool mode, CMD_action_enum action)
{
    // AT+CBATCHK=<mode>
    // AT+CBATCHK?
    BeginCMD();
    WriteCmdIntoBuff(CBATCHK, action);
    if (action == WRITE)
        WriteNumberIntoBuff((uint8_t)mode);
    EndCMD();
    return SendCMD();
}

bool SetPreferredMode(Connection_mode_enum mode)
{
    BeginCMD();
    WriteCmdIntoBuff(CNMP, WRITE);
    WriteNumberIntoBuff((uint8_t)mode);
    EndCMD();
    return SendCMD();
}

bool GetPreferredMode()
{
    BeginCMD();
    WriteCmdIntoBuff(CNMP, READ);
    EndCMD();
    return SendCMD();
}

bool Reboot()
{
    BeginCMD();
    WriteCmdIntoBuff(CREBOOT, EXE);
    EndCMD();
    return SendCMD();
}

bool WakeUpIndication(bool enable)
{
    // AT+CPSMSTATUS=<enable>
    BeginCMD();
    WriteCmdIntoBuff(CPSMSTATUS, WRITE);
    WriteNumberIntoBuff((uint8_t)enable);
    EndCMD();
    return SendCMD();
}

int PSMParameters()
{
    // AT+CPSMRDP
    BeginCMD();
    WriteCmdIntoBuff(CPSMRDP, EXE);
    EndCMD();
    if (SendCMD())
    {
        bool find = false;
        int size_sub_str = -1;
        int index = 0;
        while (!find)
        {
            if (index + SIZE(CPSMRDP) > end_msg_received)
                return -1;
            size_sub_str = StrContainsSubstr(&(msg_received[begin_msg_received + index]), CPSMRDP, msg_received_size, SIZE(CPSMRDP));
            if (size_sub_str >= 0)
            {
                index += size_sub_str + 2 + SIZE(CPSMRDP);
                if (msg_received[begin_msg_received + index - 2] == ':')
                    find = true;
                else
                    index -= 2;
            }
        }
        return (msg_received[begin_msg_received + index] == '1');
    }
    return -1;
}

bool ConfigurePSM(uint8_t psm_opt_mask, uint8_t max_oos_full_s, uint32_t psm_duration_due_to_oos, uint16_t psm_randomization_window, uint16_t max_oos_time, uint16_t early_wakeup_time)
{
    BeginCMD();
    WriteCmdIntoBuff(CPSMCFGEXT, WRITE);
    WriteNumberIntoBuff(psm_opt_mask);
    ValueDelimiter();
    WriteNumberIntoBuff(max_oos_full_s);
    ValueDelimiter();
    WriteNumberIntoBuff(psm_duration_due_to_oos);
    ValueDelimiter();
    WriteNumberIntoBuff(psm_randomization_window);
    ValueDelimiter();
    WriteNumberIntoBuff(max_oos_time);
    ValueDelimiter();
    WriteNumberIntoBuff(early_wakeup_time);
    EndCMD();
    return SendCMD();
}

bool SetThreshold(uint32_t threshold)
{
    // AT+CPSMCFG=<threshold>
    BeginCMD();
    WriteCmdIntoBuff(CPSMCFG, WRITE);
    WriteNumberIntoBuff(threshold);
    EndCMD();
    return SendCMD(10);
}

bool GetThreshold()
{
    // AT+CPSMCFG?
    BeginCMD();
    WriteCmdIntoBuff(CPSMCFG, READ);
    EndCMD();
    return SendCMD(5);
}
// ---------------------------------------------------------------------------------------

//  TCP/UDP app --------------------------------------------------------------------------
bool SetKeepaliveTCPUDP(bool kpalive_enable, int kpalive_idle, int kpalive_intval, int kpalive_cnt)
{
    BeginCMD();
    WriteCmdIntoBuff(CACFG, WRITE);
    WriteStrIntoBuff(KEEPALIVE);
    ValueDelimiter();
    WriteNumberIntoBuff((uint8_t)kpalive_enable);
    ValueDelimiter();
    WriteNumberIntoBuff(kpalive_idle);
    ValueDelimiter();
    WriteNumberIntoBuff(kpalive_intval);
    ValueDelimiter();
    WriteNumberIntoBuff(kpalive_cnt);
    EndCMD();
    return SendCMD();
}

bool GetTransmissionParameters()
{
    BeginCMD();
    WriteCmdIntoBuff(CACFG, READ);
    EndCMD();
    return SendCMD();
}

bool OpenTCPconnection(int cid, int pdp_index, const char *conn_type, const char *server, const char *port)
{
    // AT+CAOPEN=<cid>,<pdp_index>,<conn_type>,<server>, <port>[,<recv_mode>]
    BeginCMD();
    WriteCmdIntoBuff(CAOPEN, WRITE);
    WriteNumberIntoBuff(cid);
    ValueDelimiter();
    WriteNumberIntoBuff(pdp_index);
    ValueDelimiter();
    WriteStrIntoBuff(conn_type);
    ValueDelimiter();
    WriteStrIntoBuff(server);
    ValueDelimiter();
    WriteCharArrayIntoBuff(port);
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
    WriteNumberIntoBuff(pdpidx);
    ValueDelimiter();
    WriteNumberIntoBuff((uint8_t)action);
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
        char pdp = '0' + (uint8_t)pdpidx;
        int size_sub_str = StrContainsSubstr(&(msg_received[begin_msg_received]), CNACT, msg_received_size, SIZE(CNACT));
        int index = size_sub_str + begin_msg_received + 2 + SIZE(CNACT);
        for (int i = 0; i < 4; i++)
        {
            if (msg_received[index] == pdp)
                return (msg_received[index + 2] == '1');
            else
            {
                size_sub_str += StrContainsSubstr(&(msg_received[begin_msg_received + size_sub_str]), CNACT, msg_received_size - size_sub_str, SIZE(CNACT));
                index = begin_msg_received + size_sub_str + 2 + SIZE(CNACT);
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
    WriteNumberIntoBuff((uint8_t)fun);
    EndCMD();
    if (SendCMD(15))
    {
        if (StrContainsSubstr(msg_received, CPIN, MSG_RECEIVED_BUFF_SIZE, SIZE(CPIN)) >= 0 && StrContainsSubstr(msg_received, "READY", MSG_RECEIVED_BUFF_SIZE, strlen("READY")) >= 0)
        {
            // if (StrContainsSubstr(msg_received, "NOT", MSG_RECEIVED_BUFF_SIZE, strlen("NOT")) >= 0)
            // {
            //     ESP_LOGW(TAG, );
            // }
            return true;
        }
        return true; // <-----------
    }
    return false;
}

bool GetPhoneFunc()
{
    // AT+CFUN?
    BeginCMD();
    WriteCmdIntoBuff(CFUN, READ);
    EndCMD();
    return SendCMD();
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