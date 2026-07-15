#ifndef WIFI_STA_H
#define WIFI_STA_H

#include <string.h>
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_log.h"
#include "nvs_flash.h"

void WiFi_Event_Handler(void* arg, esp_event_base_t base, int32_t event_id, void* event_data);
void Wifi_Init_STA(const char * ssid, const char * password);

#endif