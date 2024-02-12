#include "../include/uart_sim7070g.h"

const uart_port_t uart_sim7070g = UART_NUM_2;
char msg_received[MSG_RECEIVED_BUFF_SIZE] = {0};
char subscribe_data[MSG_RECEIVED_BUFF_SIZE] = {0};
uint16_t msg_received_size = 0;
uint16_t begin_msg_received = 0;
uint16_t end_msg_received = 0;
bool received = false;
bool mqtt_publish_flag = false;

static const char *TAG = "Uart";
uint64_t time_last_msg = 0;

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