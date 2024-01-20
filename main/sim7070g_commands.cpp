#include "sim7070g_commands.h"

void BeginCMD()
{
    message_pointer_pos = 0;
    for (int i = 0; i < SIZE(BEGIN_CMD); i++)
    {
        message_buff[i] = BEGIN_CMD[i];
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
    message_buff[message_pointer_pos] = VALUE_DELIMITER;
    message_pointer_pos++;
}

void MQTTConnect()
{
    BeginCMD();
    
    for (int i = 0; i < SIZE(SMCONN); i++)
    {
        message_buff[message_pointer_pos] = SMCONN[i];
        message_pointer_pos++;
    }
}

void MQTTSubscribeTopic(char* topic, uint8_t qos)
{
    BeginCMD();

    for (int i = 0; i < SIZE(SMSUB); i++)
    {
        message_buff[message_pointer_pos] = SMSUB[i];
        message_pointer_pos++;
    }

    message_buff[message_pointer_pos] = WRITE_CMD;
    message_pointer_pos++;
    for (int i = 0; i < SIZE(topic); i++)
    {
        message_buff[message_pointer_pos] = topic[i];
        message_pointer_pos++;
    }

    ValueDelimite();

    message_buff[message_pointer_pos] = (char)qos;
    message_pointer_pos++;
}


void SetClientID(uint8_t id)
{
    BeginCMD();

    for (int i = 0; i < SIZE(SMCONF); i++)
    {
        message_buff[message_pointer_pos] = SMCONF[i];
        message_pointer_pos++;
    }

    QuotationMarks();
    for (int i = 0; i < SIZE(CLIENTID); i++)
    {
        message_buff[message_pointer_pos] = CLIENTID[i];
        message_pointer_pos++;
    }
    QuotationMarks();
    
    ValueDelimite();

    QuotationMarks();
    message_buff[message_pointer_pos] = (char)id;
    message_pointer_pos++;
    QuotationMarks();
}

void SetBrokerURL(char* address, uint16_t port)
{
    BeginCMD();

    for (int i = 0; i < SIZE(SMCONF); i++)
    {
        message_buff[message_pointer_pos] = SMCONF[i];
        message_pointer_pos++;
    }

    QuotationMarks();
    for (int i = 0; i < SIZE(URL); i++)
    {
        message_buff[message_pointer_pos] = URL[i];
        message_pointer_pos++;
    }
    QuotationMarks();
    
    ValueDelimite();

    QuotationMarks();
    for (int i = 0; i < SIZE(address); i++)
    {
        message_buff[message_pointer_pos] = address[i];
        message_pointer_pos++;
    }
    QuotationMarks();

    ValueDelimite();
    
    QuotationMarks();
    message_buff[message_pointer_pos] = (port & 0xff00) >> 8;
    message_pointer_pos++;
    message_buff[message_pointer_pos] = port & 0xff;
    message_pointer_pos++;
    QuotationMarks();
}