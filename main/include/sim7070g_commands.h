#ifndef SIM7070G_COMMANDS
#define SIM7070G_COMMANDS

#include <stdio.h>
#include <stdint.h>
#include "driver/uart.h"

using namespace std;

#define MESSAGE_BUFF_MAX_SIZE 559

#define BEGIN_CMD       "AT"
#define CMD_DELIMITER   ';'
#define VALUE_DELIMITER ','
#define TEST_CMD        "=?"
#define READ_CMD        '?'
#define WRITE_CMD       '='

#define SMCONF      "+SMCONF"   // Set MQTT Parameter
#define SMSSL       "+SMSSL"    // Select SSL Configure
#define SMCONN      "+SMCONN"   // MQTT Connection
#define SMPUB       "+SMPUB"    // Send Packet
#define SMSUB       "+SMSUB"    // Subscribe Packet
#define SMUNSUB     "+SMUNSUB"  // Unsubscribe Packet
#define SMDISC      "+SMDISC"   // Disconnection MQTT

#define CLIENTID    "CLIENTID"
#define URL         "URL"
#define KEEPTIME    "KEEPTIME"
#define USERNAME    "USERNAME"
#define PASSWORD    "PASSWORD"
#define CLEANSS     "CLEANSS"
#define QOS         "QOS"
#define TOPIC       "TOPIC"
#define MESSAGE     "MESSAGE"
#define RETAIN      "RETAIN"
#define SUBHEX      "SUBHEX"
#define ASYNCMODE   "ASYNCMODE"

// Responses
#define RESP_OK     "OK"
#define RESP_ERROR  "ERROR"

#define SIZE(x) (sizeof(x) / sizeof(char)) - 1

char message_buff[MESSAGE_BUFF_MAX_SIZE] = {0};
uint16_t message_pointer_pos = 0;

/**
 * Begin message buffer
*/
void BeginCMD();

/**
 * Put quotations marks in the message
*/
void QuotationMarks();

/**
 * Put ',' in the message
*/
void ValueDelimite();

/*
* Connect to MQTT broker
* @author Eduardo Veiga
* @return null
*/
void MQTTConnect();

/*
* Subscribe to MQTT broker topic
* @author Eduardo Veiga
* @param topic: char*
* @param qos: uint8_t
*/
void MQTTSubscribeTopic(char* topic, uint8_t qos = 2);

/*
* Set the client ID
* @author Eduardo Veiga
* @param id: uint8_t
*/
void SetClientID(uint8_t id);

/*
* Set the URL for connection
* @author Eduardo Veiga
* @param address: char*
* @param port: uint16_t
*/
void SetBrokerURL(char* address, uint16_t port);

/*
AT+SMCONF="CLIENTID","id"
AT+SMCONF="KEEPTIME",60
AT+SMCONF="URL","test.mosquitto.org","1883"
AT+SMCONF="CLEANSS",1
AT+SMCONF="QOS",1
AT+SMCONF="TOPIC","will topic"
AT+SMCONF="MESSAGE","will message"
AT+SMCONF="RETAIN",1
AT+SMCONF="SUBHEX",1
AT+SMCONF="ASYNCMODE",1
*/

#endif