#ifndef GSM_H_
#define GSM_H_

#include <stdio.h>
#include <stdint.h>
#include <string>
#include <iostream>
#include "../sim7070g_commands.h"
#include "../pins.h"
#include "../uart_sim7070g.h"
#include "esp_log.h"

#define DELAY_ERROR_MSG 100 / portTICK_PERIOD_MS
#define DELAY_MSG 1000 / portTICK_PERIOD_MS
#define DELAY_SLEEP_MODE 100 / portTICK_PERIOD_MS
#define ERROR_FLAG_MAX 6

#define GPS
//  #define GSM
#define MQTT
#ifdef MQTT
#ifndef GSM
#define GSM
#endif
#endif

// #define GPS

using namespace std;

extern const char *client_id;

enum MQTT_status_enum
{
    OFF = 0,
    ON = 1,
    ERROR = 2
};

class Gsm
{
private:
    char serial_num[8] = {0};

    // Flags
    volatile bool gsm_error;
    volatile bool mqtt_error;
    volatile bool gps_error;

    volatile uint8_t gsm_error_cont;
    volatile uint8_t mqtt_error_cont;
    volatile uint8_t gps_error_cont;

    bool GsmErrorFlagCount();
    void GsmErrorFlagReset();

    bool MqttErrorFlagCount();
    void MqttErrorFlagReset();

    bool GpsErrorFlagCount();
    void GpsErrorFlagReset();

public:
    Gsm();
    Gsm(const char sn[8]);
    ~Gsm();

    void Initialize();

#ifdef GSM
    bool PDNManualActivation();
#endif
#ifdef MQTT
    bool MQTTInit();
#endif
#ifdef GPS
    bool GNSSInit();
#endif
#ifdef MQTT
    bool MQTTConfig();
#endif

    char *GetSerialNumber();
    void SetSerialNumber(const char sn[8]);

    bool GetGsmErrorFlag() { return this->gsm_error; }
    bool GetMqttErrorFlag() { return this->mqtt_error; }
    bool GetGpsErrorFlag() { return this->gps_error; }

#ifdef GSM
    bool network_connect();
    bool net_connected();
#endif

#ifdef MQTT
    int mqtt_connected();
    bool mqtt_connect();
    bool mqtt_sub(char *topic);
    bool mqtt_publish(char *topic, unsigned char *msg, size_t msg_length);
    bool mqtt_publish(unsigned char *msg, size_t msg_length, int slot);
#endif
#ifdef GPS
    bool GetLocation();
#endif
#ifdef PSM
    int GetPSWMode();
    bool PowerSavingMode(bool mode);
#endif
#ifdef DTR
    bool SleepMode(bool mode);
#endif

#ifdef MQTT
    MQTT_status_enum get_mqtt_status();
#endif
    bool EchoMode(bool mode);
#ifdef GPS
    double GetLatitude();
    double GetLongitude();
    double GetAltitude();
    int GetSatellitesInView();
    void PrintCoord();
#endif
    bool PowerOn();
    void PowerOff();
};

#endif