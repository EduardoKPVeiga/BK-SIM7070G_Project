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
#define ERROR_FLAG_MAX 6

using namespace std;

enum MQTT_status_enum
{
    OFF = 0,
    ON = 1,
    ERROR = 2
};

class Gsm : public Interface
{
private:
    char serial_num[8] = {0};

    // Flags
    bool gsm_error;
    bool mqtt_error;
    bool gps_error;

    void Initialize(bool flag);

    void PDNAutoActivation();
    bool PDNManualActivation();

    bool MQTTInit();
    bool GNSSInit();
    void GPRSInit();

    bool ErrorFlagCount(bool *flag, uint8_t *count);
    void ErrorFlagReset(bool *flag, uint8_t *count);

public:
    Gsm();
    Gsm(bool flag);
    Gsm(char sn[8]);
    Gsm(char sn[8], bool flag);
    ~Gsm();

    char *GetSerialNumber();
    void SetSerialNumber(const char sn[8]);

    bool GetGsmErrorFlag() { return this->gsm_error; }
    bool GetMqttErrorFlag() { return this->mqtt_error; }
    bool GetGpsErrorFlag() { return this->gps_error; }

    bool mqtt_publish(char *topic, unsigned char *msg, size_t msg_length);
    bool mqtt_publish(unsigned char *msg, size_t msg_length, int slot);
    bool GetLocation();

    MQTT_status_enum get_mqtt_status();

    double GetLatitude();
    double GetLongitude();
    double GetAltitude();
    int GetSatellitesInView();
    void PrintCoord();
};

#endif