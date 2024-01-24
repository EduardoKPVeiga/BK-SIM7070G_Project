#ifndef SIM7070G_COMMANDS_H_
#define SIM7070G_COMMANDS_H_

#include <stdio.h>
#include <stdint.h>
#include "freertos/FreeRTOS.h"
#include "pins.h"
#include "str.h"
#include "uart_sim7070g.h"
#include "string.h"

#define BEGIN_CMD "AT\0"
#define END_CMD "\0\r\n"
#define RESP_DELIMITER "<CR><LF>"
#define CMD_DELIMITER ";"
#define VALUE_DELIMITER ","
#define TEST_CMD "=?"
#define READ_CMD "?"
#define WRITE_CMD "="

#define ECHO_OFF "AT\0E0"
#define SMCONF "+SMCONF"   // Set MQTT Parameter
#define SMSSL "+SMSSL"     // Select SSL Configure
#define SMCONN "+SMCONN"   // MQTT Connection
#define SMPUB "+SMPUB"     // Send Packet
#define SMSUB "+SMSUB"     // Subscribe Packet
#define SMUNSUB "+SMUNSUB" // Unsubscribe Packet
#define SMDISC "+SMDISC"   // Disconnection MQTT

#define CLIENTID "CLIENTID"
#define URL "URL"
#define KEEPTIME "KEEPTIME"
#define USERNAME "USERNAME"
#define PASSWORD "PASSWORD",
#define CLEANSS "CLEANSS"
#define QOS "QOS"
#define TOPIC "TOPIC"
#define MESSAGE "MESSAGE"
#define RETAIN "RETAIN"
#define SUBHEX "SUBHEX"
#define ASYNCMODE "ASYNCMODE"

// Responses
#define RESP_OK "OK"
#define RESP_ERROR "ERROR"

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

/**
 * Begin command line
 * @author Eduardo Veiga
 * @return void
 */
void BeginCMD();

/**
 * End command line
 * @author Eduardo Veiga
 * @return void
 */
void EndCMD();

/**
 * Put quotations marks in the message
 * @author Eduardo Veiga
 * @return void
 */
void QuotationMarks();

/**
 * Put ',' in the message
 * @author Eduardo Veiga
 * @return void
 */
void ValueDelimite();

/**
 * Put the character '=' in the message
 * @author Eduardo Veiga
 * @return void
 */
void WriteCMD();

/*
 * Connect to MQTT broker
 * @author Eduardo Veiga
 * @return true if successful, false otherwise
 */
bool MQTTConnect();

/*
 * Connect to MQTT broker
 * @author Eduardo Veiga
 * @return null
 */
void MQTTDisconnect();

/*
 * Subscribe to MQTT broker topic
 * @author Eduardo Veiga
 * @param topic: char*
 * @return true if successful, false otherwise
 */
bool MQTTSubscribeTopic(const char *topic);

/*
 * Set the client ID
 * @author Eduardo Veiga
 * @param id: char*
 * @param size_id: int
 * @return true if successful, false otherwise
 */
bool SetClientID(char *id, int size_id);

/*
 * Set the URL for connection
 * @author Eduardo Veiga
 * @param address: const char*
 * @param port: const char*
 */
bool SetBrokerURL(const char *address, const char *port);

/**
 * Read command, MQTT parameters
 * @author Eduardo Veiga
 * @return void
 */
bool TestCMDMQTTParameters();

/**
 * Disable echo mode
 * @return true if successful, false otherwise
 * @author Eduardo Veiga
 */
bool EchoBackOff();

/**
 * Sends a ping message to another destination
 * @author Eduardo Veiga
 * @param ipaddress: const char
 * @return true if successful, false otherwise
 */
bool PingIPV4(const char *ipaddress);

#endif