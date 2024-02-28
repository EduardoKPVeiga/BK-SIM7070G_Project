#ifndef UART_SIM7070G_H_
#define UART_SIM7070G_H_

#include "freertos/FreeRTOS.h"
#include "esp_log.h"
#include "driver/uart.h"
#include "pins.h"
#include "esp_timer.h"

#define MSG_RECEIVED_BUFF_SIZE 2 * 1024
#define UART_TASK_STACK_SIZE 4 * 1024
#define UART_TASK_PRIORITY 0
#define UART_TASK_CORE_ID 1

#define BAUD_RATE 115200

#define MESSAGE_BUFF_MAX_SIZE 559

#define DELAY_SEND 10                          // ms
#define DELAY_RECEIVED 10 / portTICK_PERIOD_MS // ms

extern char message_buff[MESSAGE_BUFF_MAX_SIZE];
extern uint16_t message_pointer_pos;
extern char msg_received[MSG_RECEIVED_BUFF_SIZE];
extern uint16_t msg_received_size;
extern uint16_t begin_msg_received;
extern uint16_t end_msg_received;
extern bool received;
extern bool enter_psm;
extern bool waiting_psm;
extern bool mqtt_publish_flag;
extern uint64_t time_last_msg;

/**
 * Initialize UART
 * @author Eduardo Veiga
 * @return void
 */
void UARTSim7070gInit();

/**
 * Clean the message received buffer
 * @author Eduardo Veiga
 * @return void
 */
void Clean_msg_received();

/**
 * Clean the message received buffer
 * @author Eduardo Veiga
 * @return void
 */
void Clean_subscribe_data();

/**
 * Send the message buffer to SIM7070G
 * @author Eduardo Veiga
 * @param max_resp_time : int (seconds)
 * @return true if successful, false otherwise
 */
bool SendCMD(int max_resp_time = 30);

/**
 * Read the uart buffer
 * @author Eduardo Veiga
 */
void ReadUart();

/**
 * Initialize uart and uart_task
 * @return void
 */
void uart2_task_init();

/**
 * UART Interrupt Service Routine task, used to receive sim7070g data
 * @return ESP_OK if successful
 */
void uart2_task(void *pvParameters);

#endif