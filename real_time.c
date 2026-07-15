#include "real_time.h"

static const char *tag = "Real time";

void Initialize_SNTP() 
{
    ESP_LOGI(tag, "Ініціалізація SNTP");
    esp_sntp_setoperatingmode(SNTP_OPMODE_POLL);
    esp_sntp_setservername(0, "pool.ntp.org");
    esp_sntp_setservername(1, "europe.pool.ntp.org");
    esp_sntp_init();
    Set_Config_SNTP();
}

void Set_Config_SNTP()
{
    int retry = 0;
    while (esp_sntp_get_sync_status() == SNTP_SYNC_STATUS_RESET && ++retry < 10) {
        vTaskDelay(pdMS_TO_TICKS(1000));;
    }
    esp_sntp_set_sync_interval(1000);
    setenv("TZ", "EET-2EEST,M3.5.0/3,M10.5.0/4", 1);
    tzset();
}

void Get_Time_from_Server(struct tm* timeinfo, char* time_str, size_t length)
{
    time_t now;
    time(&now);
    localtime_r(&now, timeinfo);
    strftime(time_str, length, "%H:%M", timeinfo);
    //ESP_LOGI(tag, "Час: %s", time_str);
}

uint16_t Get_Minutes(struct tm* timeinfo)
{
    return timeinfo->tm_hour * 60 + timeinfo->tm_min;
}

