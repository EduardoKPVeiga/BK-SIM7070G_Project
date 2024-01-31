#ifndef MAIN_H_
#define MAIN_H_

// SIM card number: (41)99292-0222

#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "sim7070g_commands.h"
#include "esp_log.h"
#include "uart_sim7070g.h"
#include <stdlib.h>

void MQTTInit();
void GNSSInit();
void GPRSInit();
double GetLatitude();
double GetLongitude();
double GetAltitude();
int GetSatellitesInView();
void PDNAutoActivation();
void PDNManualActivation();
void PrintCoord();
void CommunicationFailed();

#endif