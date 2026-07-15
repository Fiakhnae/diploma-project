#ifndef LED_H
#define LED_H

#include "driver/ledc.h"

#define LEDC_TIMER          LEDC_TIMER_0
#define LEDC_MODE           LEDC_LOW_SPEED_MODE
#define LEDC_DUTY_RES       LEDC_TIMER_8_BIT
#define LEDC_FREQ           (5000) // 5 kHz

#define PIN_RED             GPIO_NUM_12
#define PIN_GREEN           GPIO_NUM_13
#define PIN_BLUE            GPIO_NUM_14
#define PIN_WHITE           GPIO_NUM_16

void LEDC_Init();
void Led_Color_Normalize(float brightness, uint16_t* red, uint16_t* green, uint16_t* blue, uint16_t* clear);
void Set_Led_Color(uint8_t r, uint8_t g, uint8_t b, uint8_t w);


#endif