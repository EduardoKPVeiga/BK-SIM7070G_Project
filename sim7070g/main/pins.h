#ifndef PINS_H_
#define PINS_H_

#include "freertos/FreeRTOS.h"
#include <freertos/task.h>
#include "driver/gpio.h"

#define RX0 GPIO_NUM_3
#define TX0 GPIO_NUM_1
#define RX1 GPIO_NUM_16
#define TX1 GPIO_NUM_17
#define CTS1 GPIO_NUM_15
#define RTS1 GPIO_NUM_14
#define PWRKEY GPIO_NUM_27

void PinsSetup();

void PWRKEYPulse();

#endif