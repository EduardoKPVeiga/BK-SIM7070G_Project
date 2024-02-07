#include "../include/uart_sim7070g.h"
#include "../include/sim7070g_commands.h"

const uart_port_t uart_sim7070g = UART_NUM_2;
char msg_received[MSG_RECEIVED_BUFF_SIZE] = {0};
char subscribe_data[MSG_RECEIVED_BUFF_SIZE] = {0};
uint16_t msg_received_size = 0;
uint16_t begin_msg_received = 0;
uint16_t end_msg_received = 0;
bool received = false;
bool mqtt_publish_flag = false;

static const char *TAG = "Uart";

char message_buff[MESSAGE_BUFF_MAX_SIZE] = {0};
uint16_t message_pointer_pos = 0;

// Setup UART buffered IO with event queue
QueueHandle_t uart_queue;
TaskHandle_t tunnelTaskHandle = NULL;

void UARTSim7070gInit()
{
    ESP_LOGI(TAG, "Initializing uart 2");
    uart_config_t uart_config = {
        .baud_rate = BAUD_RATE,
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
    };

    // Set UART pins(TX: IO4, RX: IO5, RTS: IO18, CTS: IO19)
    ESP_ERROR_CHECK(uart_set_pin(uart_sim7070g, TX1, RX1, RTS1, CTS1));

    // Install UART driver using an event queue here
    ESP_ERROR_CHECK(uart_driver_install(uart_sim7070g, MSG_RECEIVED_BUFF_SIZE, MSG_RECEIVED_BUFF_SIZE, 10, &uart_queue, 0));

    // Configure UART parameters
    ESP_ERROR_CHECK(uart_param_config(uart_sim7070g, &uart_config));

    ESP_LOGI(TAG, "Uart 2 initialized.");
}

void Clean_msg_received()
{
    for (int i = 0; i < MSG_RECEIVED_BUFF_SIZE; i++)
        msg_received[i] = '\0';
}

void Clean_subscribe_data()
{
    for (int i = 0; i < MSG_RECEIVED_BUFF_SIZE; i++)
        subscribe_data[i] = '\0';
}

bool SendCMD(int max_resp_time)
{
    // ESP_LOGI(TAG, "message_pointer_pos: %d", message_pointer_pos);
    char message_buff_log[message_pointer_pos] = {0};

    bool has_str_end = false;
    for (int j = 0; j < message_pointer_pos; j++)
    {
        if (message_buff[j] == '\0' && j < message_pointer_pos - 3)
            message_buff_log[j] = '/';
        else
            message_buff_log[j] = message_buff[j];
        if (message_buff_log[j] == '\0')
            has_str_end = true;
    }
    if (!has_str_end)
    {
        char message_buff_log_with_str_end[message_pointer_pos + 1] = {'\0'};
        for (int i = 0; i < message_pointer_pos; i++)
            message_buff_log_with_str_end[i] = message_buff_log[i];
        ESP_LOGI(TAG, "writing command - [%s]", message_buff_log_with_str_end);
    }
    else
        ESP_LOGI(TAG, "writing command - [%s]", message_buff_log);

    uart_write_bytes(uart_sim7070g, (const char *)message_buff, message_pointer_pos);
    uart_wait_tx_done(uart_sim7070g, 10);

    // Wait for a response
    int attempts = (max_resp_time * 1000) / DELAY_SEND;
    char msg_received_LOG[MSG_RECEIVED_BUFF_SIZE] = {0};
    for (int i = 0; i < attempts; i++)
    {
        if (received || mqtt_publish_flag)
        {
            for (int j = begin_msg_received, k = 0; j < end_msg_received; j++, k++)
            {
                if (msg_received[j] == '\0')
                    msg_received_LOG[k] = '/';
                else
                    msg_received_LOG[k] = msg_received[j];
            }

            if (StrContainsSubstr(msg_received_LOG, RESP_ERROR, msg_received_size, SIZE(RESP_ERROR)) >= 0)
            {
                ESP_LOGE(TAG, "Message Received - %s", msg_received_LOG);
                received = false;
                return 0;
            }

            ESP_LOGW(TAG, "Message Received - %s", msg_received_LOG);
            received = false;
            return 1;
        }

        vTaskDelay(DELAY_SEND / portTICK_PERIOD_MS);
    }
    ESP_LOGE(TAG, "Command failed!");
    return 0;
}

void ReadUart()
{
    char uart_read[MSG_RECEIVED_BUFF_SIZE] = {0};
    int uart_size = 0;
    ESP_ERROR_CHECK(uart_get_buffered_data_len(uart_sim7070g, (size_t *)&uart_size));
    if (uart_size > 0)
    {
        ESP_LOGI(TAG, "Message size: %d", uart_size);
        char uart_read_aux[uart_size] = {0};
        uart_read_bytes(uart_sim7070g, uart_read, uart_size, 100);

        ESP_LOGI(TAG, "%s", uart_read);
        ESP_ERROR_CHECK(uart_flush(uart_sim7070g));
    }
}

void uart2_task_init()
{
    UARTSim7070gInit();
    xTaskCreatePinnedToCore(&uart2_task, "uart2 task", MSG_RECEIVED_BUFF_SIZE, (void *)uart_sim7070g, 20, &tunnelTaskHandle, 1);
}

void uart2_task(void *pvParameters)
{
    ESP_LOGI(TAG, "Initializing uart 2 task");
    uart_event_t event;
    ESP_LOGI(TAG, "uart_event_task init");
    uint16_t old_begin = 0;
    bool big_receive = false;
    for (;;)
    {
        if (xQueueReceive(uart_queue, (void *)&event, portMAX_DELAY))
        {
            switch (event.type)
            {
            case UART_DATA:
                old_begin = begin_msg_received;
                begin_msg_received = end_msg_received;
                if (end_msg_received + event.size <= MSG_RECEIVED_BUFF_SIZE)
                {
                    end_msg_received += event.size;
                }
                else
                {
                    begin_msg_received = 0;
                    end_msg_received = event.size;
                }
                msg_received_size = event.size;
                uart_read_bytes(uart_sim7070g, &(msg_received[begin_msg_received]), msg_received_size, portMAX_DELAY);
                uart_flush_input(uart_sim7070g);
                vTaskDelay(DELAY_RECEIVED);
                received = true;
                if (StrContainsSubstr(&(msg_received[begin_msg_received]), SMSUB, msg_received_size, SIZE(SMSUB)) >= 0)
                {
                    Clean_subscribe_data();
                    for (int j = begin_msg_received, k = 0; j < 4; j++, k++)
                    {
                        if (msg_received[j] == '\0')
                            subscribe_data[k] = '/';
                        else
                            subscribe_data[k] = msg_received[j];
                    }
                    ESP_LOGI(TAG, "%s", subscribe_data);
                }
                if (!StrContainsChar(&(msg_received[begin_msg_received]), '>', msg_received_size))
                {
                    if (StrContainsSubstr(&(msg_received[begin_msg_received]), RESP_OK, msg_received_size, SIZE(RESP_OK)) < 0 && StrContainsSubstr(&(msg_received[begin_msg_received]), RESP_ERROR, msg_received_size, SIZE(RESP_ERROR)) < 0)
                    {
                        big_receive = true;
                        received = false;
                    }
                    else
                    {
                        if (big_receive)
                        {
                            begin_msg_received = old_begin;
                            msg_received_size = end_msg_received - begin_msg_received;
                            big_receive = false;
                        }
                    }
                }
                else
                {
                    mqtt_publish_flag = true;
                }
                break;

            case UART_FIFO_OVF:
                ESP_LOGI(TAG, "hw fifo overflow");
                uart_flush_input(uart_sim7070g);
                xQueueReset(uart_queue);
                break;

            case UART_BUFFER_FULL:
                ESP_LOGW(TAG, "uart buffer full");
                uart_flush_input(uart_sim7070g);
                xQueueReset(uart_queue);
                break;

            case UART_BREAK:
                ESP_LOGI(TAG, "uart rx break");
                break;

            case UART_PARITY_ERR:
                ESP_LOGI(TAG, "uart parity error");
                break;

            case UART_FRAME_ERR:
                ESP_LOGI(TAG, "uart frame error");
                break;

            default:
                ESP_LOGI(TAG, "uart event type: %d", event.type);
                break;
            }
        }
    }
    vTaskDelete(NULL);
}