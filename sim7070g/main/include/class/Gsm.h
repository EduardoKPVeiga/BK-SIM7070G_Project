#ifndef GSM_H_
#define GSM_H_

#include <stdio.h>
#include <stdint.h>
#include <string>
#include <iostream>
#include "Interface.h"
#include "../sim7070g_commands.h"
#include "../pins.h"
#include "../uart_sim7070g.h"
#include "esp_log.h"

#define DELAY_ERROR_MSG 1000 / portTICK_PERIOD_MS

using namespace std;

class Gsm : public Interface
{
private:
    char serial_num[8] = {0};

    void Initialize();

    void PDNAutoActivation();
    void PDNManualActivation();

    void MQTTInit();
    void GNSSInit();
    void GPRSInit();

public:
    Gsm();
    Gsm(char sn[8]);
    ~Gsm();

    bool mqtt_publish(char *topic, unsigned char *msg, size_t msg_length);
    bool mqtt_publish(unsigned char *msg, size_t msg_length, int slot);

    bool GetLocation();
};

#endif