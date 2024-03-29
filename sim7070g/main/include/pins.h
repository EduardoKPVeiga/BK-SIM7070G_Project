#ifndef PINS_H_
#define PINS_H_

#include "freertos/FreeRTOS.h"
#include <freertos/task.h>
#include "driver/gpio.h"

#define RX1 GPIO_NUM_3
#define TX1 GPIO_NUM_1
#define CTS1 GPIO_NUM_15
#define RTS1 GPIO_NUM_14
#define SS5 GPIO_NUM_0
#define PWRKEY GPIO_NUM_22
// #define DTR GPIO_NUM_26

#define PWRKEY_T_ON 1000 / portTICK_PERIOD_MS      // ms
#define PWRKEY_T_ON_UART 3000 / portTICK_PERIOD_MS // ms
#define PWRKEY_T_OFF 1400 / portTICK_PERIOD_MS     // ms
#define PWRKEY_T_OFF_ON 2200 / portTICK_PERIOD_MS  // ms
#define PWRKEY_T_OFF_VDD 2000 / portTICK_PERIOD_MS // ms

/**
 * Setup pins used
 * @author Eduardo Veiga
 * @return void
 */
void PinsSetup();

/**
 * Initialize SIM7070G with a pulse
 * @author Eduardo Veiga
 * @return void
 */
void PWRKEYPulse();

/**
 * Power off SIM7070G with a pulse
 * @author Eduardo Veiga
 * @return void
 */
void PWRKEYToGnd();

/**
 * Power off
 * @author Eduardo Veiga
 * @return void
 */
void PWRKEYPowerOff();

#endif