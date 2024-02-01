#ifndef SIM7070G_COMMANDS_H_
#define SIM7070G_COMMANDS_H_

#include <stdio.h>
#include <stdint.h>
#include "freertos/FreeRTOS.h"
#include "pins.h"
#include "str.h"
#include "uart_sim7070g.h"
#include "esp_log.h"

#define BEGIN_CMD "AT\0"
#define END_CMD "\0\r\n"
#define RESP_DELIMITER "<CR><LF>"
#define CMD_DELIMITER ";"
#define VALUE_DELIMITER ","
#define TEST_CMD "=?"
#define READ_CMD "?"
#define WRITE_CMD "="

#define ECHO_OFF "AT\0E0"

// MQTT commands
#define SMCONF "+SMCONF"   // Set MQTT Parameter
#define SMSSL "+SMSSL"     // Select SSL Configure
#define SMCONN "+SMCONN"   // MQTT Connection
#define SMPUB "+SMPUB"     // Send Packet
#define SMSUB "+SMSUB"     // Subscribe Packet
#define SMUNSUB "+SMUNSUB" // Unsubscribe Packet
#define SMDISC "+SMDISC"   // Disconnection MQTT
#define SMPUB "+SMPUB"     // Send packet

// GPRS
#define SNPING4 "+SNPING4" // Ping IPV4
#define CGATT "+CGATT"     // Attach or detach from GPRS Service
#define CGDCONT "+CGDCONT" // Define PDP context

// GNSS
#define CGNSINF "+CGNSINF" // GNSS navigation information parsed from NMEA sentences
#define CGNSPWR "+CGNSPWR" // GNSS power control
#define CGNSMOD "+CGNSMOD" // GNSS work mode set
#define SGNSCMD "+SGNSCMD" // GNSS command

// SIM cmds
#define CNSMOD "+CNSMOD"     // Network system mode
#define CGNAPN "+CGNAPN"     // Network APN
#define CEREG "+CEREG"       // EPS network registration status
#define CAOPEN "+CAOPEN"     // TCP/UDP connection
#define CPIN "+CPIN"         // SIM card status
#define CSQ "+CSQ"           // RF signal
#define CGATT "+CGATT"       // Check PS service
#define COPS "+COPS"         // Query information
#define CNCFG "+CNCFG"       // PDP configure
#define CBANDCFG "+CBANDCFG" // Test if the band is properly set
#define CFUN "+CFUN"         // Set Phone Functionality
#define CNBS "+CNBS"         // Band scan optimization
#define CENG "+CENG"         // Engineering mode

// SMS
#define CMGF "+CMGF" // Select SMS message format
#define CMGD "+CMGD" // Delete SMS message
#define CMGS "+CMGS" // Send SMS message
#define CMGW "+CMGW" // Write SMS message to memory
#define CMSS "+CMSS" // Send SMS message from storage

#define CNTP "+CNTP" // UTC time

#define CNACT "+CNACT" // APP network active
#define CMNB "+CMNB"   // Preferred selection between CAT-M and NB-IoT
#define CLBS "+CLBS"   // Base station location

#define CLIENTID "CLIENTID"
#define URL "URL"
#define KEEPTIME "KEEPTIME"
#define USERNAME "USERNAME"
#define PASSWORD "PASSWORD"
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
#define RESP_DEACTIVE "DEACTIVE"

enum CMD_action_enum
{
    WRITE = 0,
    READ = 1,
    TEST = 2,
    EXE = 3
};

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

enum Action_enum
{
    DEACTIVED = 0,
    ACTIVED = 1,
    AUTO_ACTIVE = 2
};

enum Network_select_enum
{
    CAT_M = 1,
    NB_IOT = 2,
    BOTH = 3
};

enum Qos_enum
{
    QOS_0 = 0,
    QOS_1 = 1,
    QOS_2 = 2
};

enum Cfun_enum
{
    MIN_FUNC = 0,
    FULL_FUNC = 1
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
 * Put the char array "=?" in the message
 * @author Eduardo Veiga
 * @return true if successful, false otherwise
 */
void TestCMD();

/**
 * Write enum as a char
 * @author Eduardo Veiga
 * @param value : uint8_t
 * @return void
 */
void EnumToCharWriteBuff(uint8_t value);

/**
 * PDP index from int to char
 * @author Eduardo Veiga
 * @param pdpidx : int
 * @return void
 */
void AddPDPIndex(int pdpidx);

/**
 * Add a char array in the message buffer
 * @author Eduardo Veiga
 * @param str : const char*
 * @return void
 */
void WriteStrIntoBuff(const char *str);

/**
 * Add a char array in the message buffer
 * @author Eduardo Veiga
 * @param cmd : const char*
 * @param action : CMD_action_enum
 * @return void
 */
void WriteCmdIntoBuff(const char *cmd, CMD_action_enum action);

/*
 * Connect to MQTT broker
 * @author Eduardo Veiga
 * @return true if successful, false otherwise
 */
bool MQTTConnect();

/*
 * Disconnect to MQTT broker
 * @author Eduardo Veiga
 * @return true if successful, false otherwise
 */
bool MQTTDisconnect();

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
 * Set the asyncmode
 * @author Eduardo Veiga
 * @param mode : bool
 * @return true if successful, false otherwise
 */
bool SetAsyncmode(bool mode);

/**
 * Set subhex
 * @author Eduardo Veiga
 * @param data_hex : bool
 * @return true if successful, false otherwise
 */
bool SetSubhex(bool data_hex);

/**
 * Set the retain mode
 * @author Eduardo Veiga
 * @param mode : bool
 * @return true if successful, false otherwise
 */
bool SetRetain(bool mode);

/**
 * Set publish message details
 * @author Eduardo Veiga
 * @param details : const char*
 * @return true if successful, false otherwise
 */
bool SetMessageDetails(const char *details);

/**
 * Set QOS level
 * @author Eduardo Veiga
 * @param level : int
 * @return true if successful, false otherwise
 */
bool SetQOS(Qos_enum level);

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
bool PingIPV4(const char *ipaddress, const char *count = "1", const char *size = "16", const char *timeout = "10000");

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
 * @param pdp_type : const char*
 * @param apn : const char*
 * @param pdp_addr : const char*
 * @param d_comp : D_comp_enum
 * @param h_comp : H_comp_enum
 * @param ipv4_ctrl : bool
 * @param emergency_flag : bool
 * @return true if successful, false otherwise
 */
bool PDPContext(const char cid, const char *pdp_type, const char *apn, const char *pdp_addr, D_comp_enum d_comp, H_comp_enum h_comp, bool ipv4_ctrl, bool emergency_flag);

/**
 * Define PDP context
 * @author Eduardo Veiga
 * @param cid : const char (1..15)
 * @param pdp_type : const char*
 * @param apn : const char*
 * @return true if successful, false otherwise
 */
bool PDPContext(const char cid, const char *pdp_type, const char *apn);

/**
 * Get PDP context
 * @author Eduardo Veiga
 * @return true if successful, false otherwise
 */
bool GetPDPContext();

/**
 * Get GNSS information
 * @author Eduardo Veiga
 * @return true if successful, false otherwise
 */
bool GetGNSS();

/**
 * Set GNSS power suply
 * @author Eduardo Veiga
 * @param state : bool
 * @return true if successful, false otherwise
 */
bool SetGNSSPowerMode(bool state);

/**
 * GNSS work mode set
 * @author Eduardo Veiga
 * @param gps_mode : bool
 * @param plo_mode : bool
 * @param bd_mode : bool
 * @param gal_mode : bool
 * @param qzss_mode : bool
 * @return true if successful, false otherwise
 */
bool SetGNSSWorkMode(bool gps_mode, bool plo_mode, bool bd_mode, bool gal_mode, bool qzss_mode);

/**
 * Set high accuracy GNSS mode
 * @author Eduardo Veiga
 * @param minInterval : const char* (default: 1000)
 * @param minDistance : const char*
 * @return true if successful, false otherwise
 */
bool SetHighAccuracyGNSSMode(const char *minInterval = "1000", const char *minDistance = "0");

/**
 * Show network system mode
 * @author Eduardo Veiga
 * @return true if successful, false otherwise
 */
bool ShowNetworkSystemMode();

/**
 * Get network APN in CAT-M or NB-IOT
 * @author Eduardo Veiga
 * @return true if successful, false otherwise
 */
bool GetNetworkAPN();

/**
 * EPS network registration status
 * @author Eduardo Veiga
 * @return true if successful, false otherwise
 */
bool GetEPSNetworkStatus();

/**
 * Set EPS network registration status
 * @author Eduardo Veiga
 * @param n : char ('0', '1', '2', '3', '4')
 * @return true if successful, false otherwise
 */
bool SetEPSNetworkStatus(char n);

/**
 * TCP/UDP connection status
 * @author Eduardo Veiga
 * @return true if successful, false otherwise
 */
bool GetTCPUDPConnectionStatus();

/**
 * Check SIM card status
 * @author Eduardo Veiga
 * @return true if successful, false otherwise
 */
bool CheckSIMCard();

/**
 * Check RF signal
 * @author Eduardo Veiga
 * @return true if successful, false otherwise
 */
bool CheckRF();

/**
 * Check PS service
 * @author Eduardo Veiga
 * @return true if sucessful, false otherwise
 */
bool CheckPSService();

/**
 * Query network information, operator and network
 * @author Eduardo Veiga
 * @return true if successful, false otherwise
 */
bool QueryNetworkInfo();

/**
 * PDP configure
 * @author Eduardo Veiga
 * @param pdpidx : int (0, 1, 2, 3)
 * @param ip_type : const char ('0', '1', '2', '3', '4')
 * @param apn : const char*
 * @return true if successful, false otherwise
 */
bool PDPConfigure(int pdpidx, const char ip_type, const char *apn);

/**
 * PDP configure read command
 * @author Eduardo Veiga
 * @return true if successful, false otherwise
 */
bool PDPConfigureReadCMD();

/**
 * Test if the band is properly set
 * @author Eduardo Veiga
 * @return true if successful, false otherwise
 */
bool BandTest();

/**
 * Preferred selection between CAT-M and NB-IoT
 * @author Eduardo Veiga
 * @param mode : Network_select_enum
 * @return true if successful, false otherwise
 */
bool SetNetworkType(Network_select_enum mode);

/**
 * Reads band scan configuration
 * @author Eduardo Veiga
 * @return true if successful, false otherwise
 */
bool GetBandScanConfig();

/**
 * Reads engineering mode information
 * @author Eduardo veiga
 * @return true if successful, false otherwise
 */
bool GetEngineeringModeInfo();

/**
 * Send packet through MQTT
 * @author Eduardo Veiga
 * @param topic : const char*
 * @param length : const char*
 * @param qos : Qos_enum
 * @param retain : bool
 * @param msg : const char*
 * @return true if successful, false otherwise
 */
bool SendPacket(const char *topic, const char *length, Qos_enum qos, bool retain, const char *msg);

/**
 * Test send packet through MQTT command
 * @author Eduardo Veiga
 * @return true if successful, false otherwise
 */
bool TestSendPacket();

/**
 * Subscribe to a MQTT topic
 * @author Eduardo Veiga
 * @param topic : const char*
 * @param qos : Qos_enum
 * @return true if successful, false otherwise
 */
bool SubscribePacket(const char *topic, Qos_enum qos);

/**
 * APP network active read command
 * @author Eduardo Veiga
 * @return true if successful, false otherwise
 */
bool AppNetworkActiveReadCMD(int pdpidx);

/**
 * Get UTC time
 * @author Eduardo Veiga
 * @return true if successful, false oherwise
 */
bool GetSynchronizeUTCTime();

/**
 * Open wireless connection parameter 0 is PDP index, parameter 1 means active.
 * @author Eduardo Veiga
 * @param pdpidx : int
 * @param action : Action_enum
 * @return true if successful, false otherwise
 */
bool APPNetworkActive(int pdpidx, Action_enum action);

/**
 * Set phone functionality
 * @author Eduardo Veiga
 * @param fun : Cfun_enum
 * @return true if successful, false otherwise
 */
bool SetPhoneFunc(Cfun_enum fun);

/**
 * Get coordinates from Base Station Location
 * @author Eduardo Veiga
 * @param type : int
 * @param cid : const char
 * @return true if successful, false otherwise
 */
bool GetGSMLocation(int type, const char cid);
#endif