#ifndef TCS34725_H
#define TCS34725_H

#include <math.h>
#include "esp_log.h"
#include "i2c_module.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"


#define I2C_TCS34725_ADDR 0x29

#define CMD_BIT  0x80

#define ENABLE       0x00
#define ENABLE_PON   0x01
#define ENABLE_AEN   0x02

#define ATIME        0x01
#define CONTROL      0x0F

#define CDATAL       0x14

#define INTEGRATIONTIME_154MS  0xC0
#define GAIN_16X               0x02

typedef struct {
    uint16_t clear;
    uint16_t red;
    uint16_t green;
    uint16_t blue;
} tcs34725_data_t;

void TCS34725_Add_To_I2C_Bus();
void TCS34725_Send_CMD(uint8_t reg, uint8_t value);
void TCS34725_Init();

uint16_t TCS34725_Read_I2C_Bus(uint8_t reg);
uint16_t TCS34725_RGBC_to_Lux();
tcs34725_data_t* TCS34725_Get_RGBC_Data();
uint16_t TCS34725_RGBC_to_Color_Temperature();
void TCS34725_Color_Temperature_to_RGBC(tcs34725_data_t* rgbc_values, uint16_t color_temperature);

#endif