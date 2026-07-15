#ifndef REAL_TIME_H
#define REAL_TIME_H

#include "esp_log.h"
#include "esp_sntp.h"

void Initialize_SNTP();
void Set_Config_SNTP();
void Get_Time_from_Server(struct tm* timeinfo, char* time_str, size_t length);
uint16_t Get_Minutes(struct tm* timeinfo);

#endif