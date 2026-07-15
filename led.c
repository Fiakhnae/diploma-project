#include "led.h"

void LEDC_Init() 
{
    ledc_timer_config_t timer_conf = {
        .speed_mode = LEDC_MODE,
        .duty_resolution = LEDC_DUTY_RES,
        .timer_num = LEDC_TIMER,
        .freq_hz = LEDC_FREQ,
        .clk_cfg = LEDC_AUTO_CLK
    };
    ESP_ERROR_CHECK(ledc_timer_config(&timer_conf));

    ledc_channel_config_t channel_conf = {
        .speed_mode = LEDC_MODE,
        .timer_sel = LEDC_TIMER,
        .intr_type = LEDC_INTR_DISABLE,
        .duty = 0,
        .hpoint = 0
    };

    channel_conf.channel = LEDC_CHANNEL_0;
    channel_conf.gpio_num = PIN_RED;
    ESP_ERROR_CHECK(ledc_channel_config(&channel_conf));

    channel_conf.channel = LEDC_CHANNEL_1;
    channel_conf.gpio_num = PIN_GREEN;
    ESP_ERROR_CHECK(ledc_channel_config(&channel_conf));

    channel_conf.channel = LEDC_CHANNEL_2;
    channel_conf.gpio_num = PIN_BLUE;
    ESP_ERROR_CHECK(ledc_channel_config(&channel_conf));

    // Для white-каналу
    // channel_conf.channel = LEDC_CHANNEL_3;
    // channel_conf.gpio_num = PIN_WHITE;
    // ESP_ERROR_CHECK(ledc_channel_config(&channel_conf));
}

void Led_Color_Normalize(float brightness, uint16_t* red, uint16_t* green, uint16_t* blue, uint16_t* clear)
{
    
    /*float brightness = 1.0f;
    if(lux > 500) brightness = 0.3f; // Зменшити яскравість при сильному світлі
    else if(lux < 10) brightness = 1.0f; // Повна яскравість у темряві*/

    // Нормалізація кольорів через C-канал
    float scale = (*clear > 0) ? (255.0f / *clear) : 0;
    *red = (*red * scale * brightness);
    *green = (*green * scale * brightness);
    *blue = (*blue * scale * brightness);

    *red = (*red > 255) ? 255 : *red;
    *green = (*green > 255) ? 255 : *green;
    *blue = (*blue > 255) ? 255 : *blue;
}

/*
// Оновити кольори стрічки
set_led_color(red, green, blue, white);

vTaskDelay(pdMS_TO_TICKS(100)); // Оновлення кожні 100 мс
*/

void Set_Led_Color(uint8_t red, uint8_t green, uint8_t blue, uint8_t white) 
{
    ledc_set_duty(LEDC_MODE, LEDC_CHANNEL_0, red);
    ledc_update_duty(LEDC_MODE, LEDC_CHANNEL_0);

    ledc_set_duty(LEDC_MODE, LEDC_CHANNEL_1, green);
    ledc_update_duty(LEDC_MODE, LEDC_CHANNEL_1);

    ledc_set_duty(LEDC_MODE, LEDC_CHANNEL_2, blue);
    ledc_update_duty(LEDC_MODE, LEDC_CHANNEL_2);

    // Для white-каналу
    // ledc_set_duty(LEDC_MODE, LEDC_CHANNEL_3, white);
    // ledc_update_duty(LEDC_MODE, LEDC_CHANNEL_3);
}