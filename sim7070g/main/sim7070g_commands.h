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
#define OPEN_BRACKET "["
#define CLOSE_BRACKET "]"
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

#define SNPING4 "+SNPING4" // Ping IPV4
#define CGATT "+CGATT"     // Attach or detach from GPRS Service
#define CGDCONT "+CGDCONT" // Define PDP context

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

enum PDP_type_enum
{
    IP = 0,     // Internet Protocol (IETF STD 5)
    IPV6 = 1,   // Internet Protocol Version 6
    IPV4V6 = 2, // Dual PDN Stack
    NON_IP = 3  // Transfer of Non-IP data to external packet dataNetwork
};

enum D_comp_enum
{
    D_OFF = 0, // Default value
    D_ON = 1,
    V42BIS = 2
};

enum H_comp_enum
{
    H_OFF = 0, // Default value
    H_ON = 1,
    RFC1144 = 2,
    RFC2507 = 3,
    RFC3095 = 4
};

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
void ValueDelimiter();

/**
 * Put the character '=' in the message
 * @author Eduardo Veiga
 * @return void
 */
void WriteCMD();

/**
 * Put the character '?' in the message
 * @author Eduardo Veiga
 * @return void
 */
void ReadCMD();

/**
 * Put the character '[' in the message
 * @author Eduardo Veiga
 * @return void
 */
void OpenBracket();

/**
 * Put the character ']' in the message
 * @author Eduardo Veiga
 * @return void
 */
void CloseBracket();

/**
 * Write enum as a char
 * @author Eduardo Veiga
 * @param value : uint8_t
 * @return void
 */
void EnumToCharWriteBuff(uint8_t value);

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
bool SetClientID(const char *id);

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
bool PingIPV4(const char *ipaddress, const char *count = "4", const char *size = "16", const char *timeout = "10000");

/**
 * GPRS attachment command
 * @author Eduardo Veiga
 * @param active : bool (0 means detached and 1 means attached)
 * @return true if successful, false otherwise
 */
bool GPRSAttachment(bool active);

/**
 * Define PDP context
 * @author Eduardo Veiga
 * @param cid : const char (1..15)
 * @param pdp_type : PDP_type_enum
 * @param apn : const char*
 * @param pdp_addr : const char*
 * @param d_comp : D_comp_enum
 * @param h_comp : H_comp_enum
 * @param ipv4_ctrl : bool
 * @param emergency_flag : bool
 * @return true if successful, false otherwise
 */
bool PDPContext(const char cid, PDP_type_enum pdp_type, const char *apn, const char *pdp_addr, D_comp_enum d_comp = D_OFF, H_comp_enum h_comp = H_OFF, bool ipv4_ctrl = true, bool emergency_flag = false);

/**
 * Get PDP context
 * @author Eduardo Veiga
 * @return true if successful, false otherwise
 */
bool GetPDPContext();

#endif