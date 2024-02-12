#ifndef GSM_H_
#define GSM_H_

#define TINY_GSM_MODEM_SIM7080

#include <TinyGsmClient.h>
#include <PubSubClient.h>
#include <HardwareSerial.h>
#include <stdio.h>
#include <stdint.h>
#include <string>
#include <iostream>
#include "Interface.h"
#include "../pins.h"
#include "../uart_sim7070g.h"
#include "esp_log.h"

#define DELAY_ERROR_MSG 100 / portTICK_PERIOD_MS
#define ERROR_FLAG_MAX 6

#define BAUD 115200

#define TINY_GSM_USE_GPRS false
#define TINY_GSM_USE_WIFI true
#define PWRKEY GPIO_NUM_27
#define PWRKEY_T_ON 1000 / portTICK_PERIOD_MS      // ms
#define PWRKEY_T_ON_UART 3000 / portTICK_PERIOD_MS // ms

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

    HardwareSerial SerialAT(1);

    TinyGsm modem(SerialAT);
    TinyGsmClient client(modem);
    PubSubClient mqtt(client);

    void Initialize(bool flag);

    bool ErrorFlagCount(bool *flag, uint8_t *count);
    void ErrorFlagReset(bool *flag, uint8_t *count);

    void mqttCallback(char *topic, byte *payload, unsigned int len);
    boolean mqttConnect();

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

    bool mqtt_publish(char *topic, unsigned char *msg, size_t msg_length);
    bool mqtt_publish(unsigned char *msg, size_t msg_length, int slot);
    bool GetLocation();
    int GetPSWMode();

    double GetLatitude();
    double GetLongitude();
    double GetAltitude();
    int GetSatellitesInView();
    void PrintCoord();
};

#endif