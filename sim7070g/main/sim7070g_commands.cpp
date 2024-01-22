#include "sim7070g_commands.h"

char message_buff[MESSAGE_BUFF_MAX_SIZE] = {0};
uint16_t message_pointer_pos = 0;

const uart_port_t uart_sim7070g = UART_NUM_1;

// UART ==================================================================================

void UARTSim7070gInit()
{
    uart_config_t uart_config = {
        .baud_rate = 115200,
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_CTS_RTS,
        .rx_flow_ctrl_thresh = 122,
    };

    // Configure UART parameters
    ESP_ERROR_CHECK(uart_param_config(uart_sim7070g, &uart_config));

    // Set UART pins(TX: IO4, RX: IO5, RTS: IO18, CTS: IO19)
    ESP_ERROR_CHECK(uart_set_pin(UART_NUM_1, TX1, RX1, RTS1, CTS1));

    // Setup UART buffered IO with event queue
    const int uart_buffer_size = (1024 * 2);
    QueueHandle_t uart_queue;

    // Install UART driver using an event queue here
    ESP_ERROR_CHECK(uart_driver_install(UART_NUM_1, uart_buffer_size, uart_buffer_size, 10, &uart_queue, 0));
}

bool SendCMD()
{
    uart_write_bytes(uart_sim7070g, (const char *)message_buff, message_pointer_pos);
    vTaskDelay(10 / portTICK_PERIOD_MS);
    uint16_t length = 0;
    ESP_ERROR_CHECK(uart_get_buffered_data_len(uart_sim7070g, (size_t *)&length));
    length = uart_read_bytes(uart_sim7070g, message_buff, length, 100);
    message_pointer_pos = length;
    uart_flush(uart_sim7070g);

    char begin_resp[SIZE(RESP_DELIMITER) + 1] = {0};
    for (int i = 0; i < SIZE(RESP_DELIMITER); i++)
    {
        begin_resp[i] = message_buff[i];
    }
    begin_resp[SIZE(RESP_DELIMITER)] = '\0';

    if (!StrCompare(begin_resp, RESP_DELIMITER))
        return 0;

    char resp[length - (2 * SIZE(RESP_DELIMITER)) + 1] = {0};
    for (int i = SIZE(RESP_DELIMITER), j = 0; i < length - SIZE(RESP_DELIMITER); i++, j++)
    {
        resp[j] = message_buff[i];
    }
    resp[length - (2 * SIZE(RESP_DELIMITER))] = '\0';

    if (!StrCompare(resp, RESP_OK))
        return 0;
    return 1;
}

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
    message_pointer_pos = 0;
    for (int i = 0; i < SIZE(END_CMD); i++)
    {
        message_buff[i] = END_CMD[i];
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

// COMMANDS ==============================================================================

void MQTTConnect()
{
    // AT+SMCONN
    BeginCMD();

    for (int i = 0; i < SIZE(SMCONN); i++)
    {
        message_buff[message_pointer_pos] = SMCONN[i];
        message_pointer_pos++;
    }

    EndCMD();

    bool send = SendCMD();
    int cont = 0;
    while (!send && cont < 3)
    {
        vTaskDelay(100 / portTICK_PERIOD_MS);
        send = SendCMD();
        cont++;
    }
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

void MQTTSubscribeTopic(char *topic, uint8_t qos = 2)
{
    // AT+SMSUB="information",1
    BeginCMD();

    for (int i = 0; i < SIZE(SMSUB); i++)
    {
        message_buff[message_pointer_pos] = SMSUB[i];
        message_pointer_pos++;
    }

    message_buff[message_pointer_pos] = WRITE_CMD[0];
    message_pointer_pos++;
    for (int i = 0; i < SIZE(topic); i++)
    {
        message_buff[message_pointer_pos] = topic[i];
        message_pointer_pos++;
    }

    ValueDelimite();

    message_buff[message_pointer_pos] = (char)qos;
    message_pointer_pos++;

    EndCMD();

    SendCMD();
}

void SetClientID(uint8_t id)
{
    // AT+SMCONF="CLIENTID","id"
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

    EndCMD();

    SendCMD();
}

void SetBrokerURL(char *address, uint16_t port)
{
    // AT+SMCONF="URL","test.mosquitto.org","1883"
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

    EndCMD();

    SendCMD();
}

void TestCMDMQTTParameters()
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

    uart_write_bytes(uart_sim7070g, (const char *)message_buff, message_pointer_pos);
    vTaskDelay(10 / portTICK_PERIOD_MS);
    uint16_t length = 0;
    ESP_ERROR_CHECK(uart_get_buffered_data_len(uart_sim7070g, (size_t *)&length));
    length = uart_read_bytes(uart_sim7070g, message_buff, length, 100);
    message_pointer_pos = length;
    uart_flush(uart_sim7070g);
}