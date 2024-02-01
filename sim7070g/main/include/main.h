#ifndef MAIN_H_
#define MAIN_H_

// SIM card number: (41)99292-0222

#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "sim7070g_commands.h"
#include "esp_log.h"
#include "uart_sim7070g.h"
#include <stdlib.h>

#define DELAY_ERROR_MSG 1000 / portTICK_PERIOD_MS

/**
 * Configure and initialize MQTT
 * @author Eduardo Veiga
 * @return void
 */
void MQTTInit();

/**
 * Configure and initialize GPS
 * @author Eduardo Veiga
 * @return void
 */
void GNSSInit();

/**
 * Configure and initialize GPRS
 * @author Eduardo Veiga
 * @return void
 */
void GPRSInit();

/**
 * Extract the latitude from message received
 * @author Eduardo Veiga
 * @return double
 */
double GetLatitude();

/**
 * Extract the longitude from message received
 * @author Eduardo Veiga
 * @return double
 */
double GetLongitude();

/**
 * Extract the altitude from message received
 * @author Eduardo Veiga
 * @return double
 */
double GetAltitude();

/**
 * Extract the numbers of satellites available from message received
 * @author Eduardo Veiga
 * @return int
 */
int GetSatellitesInView();

/**
 * Initialize GSM communication automatically
 * @author Eduardo Veiga
 * @return void
 */
void PDNAutoActivation();

/**
 * Configure and initialize GSM communication
 * @author Eduardo Veiga
 * @return void
 */
void PDNManualActivation();

/**
 * ESP_LOGI the coordinate information
 * @author Eduardo Veiga
 * @return void
 */
void PrintCoord();

#endif