#include "../include/pins.h"

void PinsSetup()
{
    gpio_set_direction(PWRKEY, GPIO_MODE_OUTPUT);
    gpio_set_level(PWRKEY, 1);
}

void PWRKEYPulse()
{
    gpio_set_level(PWRKEY, 0);
    vTaskDelay(PWRKEY_T_ON);
    gpio_set_level(PWRKEY, 1);
    vTaskDelay(PWRKEY_T_ON_UART);
    vTaskDelay(PWRKEY_T_ON);
}

void PWRKEYToGnd()
{
    gpio_set_level(PWRKEY, 0);
    vTaskDelay(PWRKEY_T_OFF);
    gpio_set_level(PWRKEY, 1);
    vTaskDelay(PWRKEY_T_OFF_VDD);
}

void PWRKEYPowerOff()
{
    gpio_set_level(PWRKEY, 0);
    vTaskDelay(PWRKEY_T_OFF);
    gpio_set_level(PWRKEY, 1);
    vTaskDelay(PWRKEY_T_OFF_VDD);
    vTaskDelay(PWRKEY_T_OFF_ON);
}