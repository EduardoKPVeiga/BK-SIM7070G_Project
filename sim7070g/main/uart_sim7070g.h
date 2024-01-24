#ifndef UART_SIM7070G_H_
#define UART_SIM7070G_H_

#include "freertos/FreeRTOS.h"
#include "esp_log.h"
#include "driver/uart.h"
#include "pins.h"

#define MSG_RECEIVED_BUFF_SIZE 2048
#define UART_TASK_STACK_SIZE 4 * 1024
#define UART_TASK_PRIORITY 0
#define UART_TASK_CORE_ID 1

#define MESSAGE_BUFF_MAX_SIZE 559

#define DELAY_SEND 100 // ms

extern char message_buff[MESSAGE_BUFF_MAX_SIZE];
extern uint16_t message_pointer_pos;

/**
 * Initialize UART
 * @author Eduardo Veiga
 * @return void
 */
void UARTSim7070gInit();

/**
 * Send the message buffer to SIM7070G
 * @author Eduardo Veiga
 * @param max_resp_time : int (seconds)
 * @return true if successful, false otherwise
 */
bool SendCMD(int max_resp_time = 10);

#endif