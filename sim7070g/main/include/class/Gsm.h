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

#define DELAY_ERROR_MSG 100 / portTICK_PERIOD_MS
#define DELAY_MSG 1000 / portTICK_PERIOD_MS
#define DELAY_SLEEP_MODE 100 / portTICK_PERIOD_MS
#define ERROR_FLAG_MAX 6

using namespace std;

extern const char *client_id;

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

    uint8_t gsm_error_cont;
    uint8_t mqtt_error_cont;
    uint8_t gps_error_cont;

    void Initialize(bool flag);

    bool PDNManualActivation();
    bool MQTTInit();
    bool GNSSInit();

    bool MQTTConfig();

    bool ErrorFlagCount(bool *flag, uint8_t *cont);
    void ErrorFlagReset(bool *flag, uint8_t *cont);

public:
    Gsm();
    Gsm(bool flag);
    Gsm(const char sn[8]);
    Gsm(const char sn[8], bool flag);
    ~Gsm();

    char *GetSerialNumber();
    void SetSerialNumber(const char sn[8]);

    bool GetGsmErrorFlag() { return this->gsm_error; }
    bool GetMqttErrorFlag() { return this->mqtt_error; }
    bool GetGpsErrorFlag() { return this->gps_error; }

    bool network_connect();
    bool net_connected();

    int mqtt_connected();
    bool mqtt_connect();
    bool mqtt_sub(char *topic);
    bool mqtt_publish(char *topic, unsigned char *msg, size_t msg_length);
    bool mqtt_publish(unsigned char *msg, size_t msg_length, int slot);

    bool GetLocation();

#ifdef PSM
    int GetPSWMode();
    bool PowerSavingMode(bool mode);
#endif
#ifdef DTR
    bool SleepMode(bool mode);
#endif

    MQTT_status_enum get_mqtt_status();

    bool EchoMode(bool mode);

    double GetLatitude();
    double GetLongitude();
    double GetAltitude();
    int GetSatellitesInView();
    void PrintCoord();
};

#endif