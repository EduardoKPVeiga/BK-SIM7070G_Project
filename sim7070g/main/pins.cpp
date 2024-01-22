#include "pins.h"

void PinsSetup()
{
    gpio_set_direction(PWRKEY, GPIO_MODE_OUTPUT);
}

void PWRKEYPulse()
{
    gpio_set_level(PWRKEY, 0);
    vTaskDelay(5 / portTICK_PERIOD_MS);
    gpio_set_level(PWRKEY, 1);
    vTaskDelay(5 / portTICK_PERIOD_MS);
    gpio_set_level(PWRKEY, 0);
}