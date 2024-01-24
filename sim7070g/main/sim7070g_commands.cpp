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

    QuotationMarks();
    for (int i = 0; i < strlen(address); i++)
    {
        message_buff[message_pointer_pos] = address[i];
        message_pointer_pos++;
    }
    QuotationMarks();

    ValueDelimiter();

    QuotationMarks();
    for (int i = 0; i < strlen(port); i++)
    {
        message_buff[message_pointer_pos] = port[i];
        message_pointer_pos++;
    }
    QuotationMarks();

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

    if (active)
        message_buff[message_pointer_pos] = '1';
    else
        message_buff[message_pointer_pos] = '0';
    message_pointer_pos++;

    EndCMD();
    return SendCMD(75);
}

bool PDPContext(const char cid, PDP_type_enum pdp_type, const char *apn, const char *pdp_addr, D_comp_enum d_comp, H_comp_enum h_comp, bool ipv4_ctrl, bool emergency_flag)
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

    EnumToCharWriteBuff(pdp_type);

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
    return SendCMD(50);
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