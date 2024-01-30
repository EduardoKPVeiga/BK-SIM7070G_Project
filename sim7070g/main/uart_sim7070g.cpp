#include "uart_sim7070g.h"
#include "sim7070g_commands.h"

const uart_port_t uart_sim7070g = UART_NUM_2;
const int uart_buffer_size = (1024 * 2);
char msg_received[MSG_RECEIVED_BUFF_SIZE] = {0};

const char *TAG = "UART2";

char message_buff[MESSAGE_BUFF_MAX_SIZE] = {0};
uint16_t message_pointer_pos = 0;

void UARTSim7070gInit()
{
    uart_config_t uart_config = {
        .baud_rate = BAUD_RATE,
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
    };

    // Set UART pins(TX: IO4, RX: IO5, RTS: IO18, CTS: IO19)
    ESP_ERROR_CHECK(uart_set_pin(uart_sim7070g, TX1, RX1, RTS1, CTS1));

    // Setup UART buffered IO with event queue
    QueueHandle_t uart_queue;

    // Install UART driver using an event queue here
    ESP_ERROR_CHECK(uart_driver_install(uart_sim7070g, uart_buffer_size, uart_buffer_size, 10, &uart_queue, 0));

    // Configure UART parameters
    ESP_ERROR_CHECK(uart_param_config(uart_sim7070g, &uart_config));
}

void Clean_msg_received()
{
    for (int i = 0; i < MSG_RECEIVED_BUFF_SIZE; i++)
        msg_received[i] = '\0';
}

bool SendCMD(int max_resp_time)
{
    Clean_msg_received();
    ESP_ERROR_CHECK(uart_flush(uart_sim7070g));
    ESP_LOGI(TAG, "message_pointer_pos: %d", message_pointer_pos);
    char message_buff_log[message_pointer_pos] = {0};

    for (int j = 0; j < message_pointer_pos; j++)
    {
        if (message_buff[j] == '\0' && j < message_pointer_pos - 3)
            message_buff_log[j] = '/';
        else
            message_buff_log[j] = message_buff[j];
    }
    ESP_LOGI(TAG, "writing command - %s", message_buff_log);

    uart_write_bytes(uart_sim7070g, (const char *)message_buff, message_pointer_pos);
    // uart_wait_tx_done(uart_sim7070g, 100);
    ESP_ERROR_CHECK(uart_flush(uart_sim7070g));
    vTaskDelay(100 / portTICK_PERIOD_MS);

    // Wait for a response
    int attempts = (max_resp_time * 1000) / DELAY_SEND;
    char msg_received_LOG[MSG_RECEIVED_BUFF_SIZE] = {0};
    uint16_t length = 0;
    for (int i = 0; i < attempts; i++)
    {
        ESP_ERROR_CHECK(uart_get_buffered_data_len(uart_sim7070g, (size_t *)&length));
        if (length > 1)
        {
            ESP_LOGW(TAG, "length: %d", length);
            uart_read_bytes(uart_sim7070g, msg_received, length, 100);
            message_pointer_pos = length;

            // if (ValidCharArray(message_buff, message_pointer_pos))
            //{

            for (int j = 0; j < length; j++)
            {
                if (msg_received[j] == '\0' && j < length - 3)
                    msg_received_LOG[j] = '/';
                else
                    msg_received_LOG[j] = msg_received[j];
            }

            if (StrContainsSubstr(msg_received, RESP_ERROR, length, SIZE(RESP_ERROR)))
            {
                ESP_LOGE(TAG, "Message Received - %s", msg_received_LOG);
                ESP_ERROR_CHECK(uart_flush(uart_sim7070g));
                return 0;
            }

            ESP_LOGW(TAG, "Message Received - %s", msg_received_LOG);
            ESP_ERROR_CHECK(uart_flush(uart_sim7070g));
            return 1;
            //}
        }
        vTaskDelay(DELAY_SEND / portTICK_PERIOD_MS);
    }
    ESP_LOGE(TAG, "Command failed!");
    ESP_ERROR_CHECK(uart_flush(uart_sim7070g));
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
        uart_read_bytes(uart_sim7070g, uart_read, 5, 100);

        ESP_LOGI(TAG, "%s", uart_read);
        ESP_ERROR_CHECK(uart_flush(uart_sim7070g));
    }
}