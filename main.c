#include <stdio.h>
#include <inttypes.h>
#include "sdkconfig.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"

#include "gy_30.h"
#include "tcs34725.h"
#include "st7789.h"
#include "wifi_sta.h"
#include "real_time.h"
#include "led.h"
#include "neural_network.h"
#include "esp_pm.h"

static const char * tag = "Main";

void app_main()
{
    lv_obj_t *lux_label = NULL;
    lv_obj_t *color_label = NULL;
    lv_obj_t *time_label = NULL;
    lv_obj_t *predicted_color_label = NULL;

    const char * lux_fmt = "Lux: %d";
    const char * color_fmt = "Color temp: %dK";
    const char * pred_color_fmt = "New color: %dK";
    const uint8_t current_time_fmt_size = 40;
    
    struct tm timeinfo;
    tcs34725_data_t color_data;

    uint16_t lux = 0;
    uint16_t color = 0;
    uint16_t predicted_color = 0;

    esp_pm_config_t pm_config = {
        .max_freq_mhz = 240,
        .min_freq_mhz = 40,
        .light_sleep_enable = true,
    };

    esp_pm_configure(&pm_config);

    I2C_Bus_Init();
    BH1750FVI_Init();
    TCS34725_Init();
    ST7789_Init();
    ST7789_LVGL_Init();
    Wifi_Init_STA("SSID", "Password");
    Initialize_SNTP();
    LEDC_Init();
    Model_Init();

    ST7789_Create_Label(&lux_label, LV_ALIGN_CENTER, 0, -20);
    ST7789_Create_Label(&color_label, LV_ALIGN_CENTER, 0, 0);
    ST7789_Create_Label(&time_label, LV_ALIGN_CENTER, 0, 20);
    ST7789_Create_Label(&predicted_color_label, LV_ALIGN_CENTER, 0, 40);

    

    xTaskCreate(lvgl_task, "LVGL", 4096*4, NULL, 2, NULL);

    for(;;)
    {
        lux = BH1750FVI_Read_I2C_Bus();
        color = TCS34725_RGBC_to_Color_Temperature();

        char current_time[15];
        Get_Time_from_Server(&timeinfo, current_time, sizeof(current_time));

        uint16_t minutes = Get_Minutes(&timeinfo);
        
        predicted_color = Predict_Color_Temperature(minutes, lux, color);
        //ESP_LOGI(tag, "Predicted color: %d", predicted_color);

        char current_time_fmt [current_time_fmt_size];
        snprintf(current_time_fmt, current_time_fmt_size, "Time: %s", current_time);

        ST7789_Show_Text(lux_label, lux_fmt, lux); 
        ST7789_Show_Text(color_label, color_fmt, color);
        ST7789_Show_Text(time_label, current_time_fmt, 0);
        ST7789_Show_Text(predicted_color_label, pred_color_fmt, predicted_color);

        TCS34725_Color_Temperature_to_RGBC(&color_data, predicted_color);
        Led_Color_Normalize(lux, &color_data.red, &color_data.green, &color_data.blue, &color_data.clear);
        Set_Led_Color(color_data.red, color_data.green, color_data.blue, color_data.clear);
    };
}
