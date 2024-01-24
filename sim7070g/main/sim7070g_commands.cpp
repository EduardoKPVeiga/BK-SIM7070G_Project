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

void ValueDelimite()
{
    message_buff[message_pointer_pos] = VALUE_DELIMITER[0];
    message_pointer_pos++;
}

void WriteCMD()
{
    message_buff[message_pointer_pos] = WRITE_CMD[0];
    message_pointer_pos++;
}

// COMMANDS ==============================================================================

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

    ValueDelimite();

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

bool SetClientID(char *id, int size_id)
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

    ValueDelimite();

    QuotationMarks();
    for (int i = 0; i < size_id; i++)
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

    ValueDelimite();

    QuotationMarks();
    for (int i = 0; i < strlen(address); i++)
    {
        message_buff[message_pointer_pos] = address[i];
        message_pointer_pos++;
    }
    QuotationMarks();

    ValueDelimite();

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