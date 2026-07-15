#ifndef GY_30_H
#define GY_30_H

#include "esp_log.h"
#include "i2c_module.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#define I2C_BH1750FVI_ADDR 0x23

#define POWER_ON    0x01
#define RESET       0x07
#define CONT_H_MODE 0x10

void BH1750FVI_Add_To_I2C_Bus();
void BH1750FVI_Init();
void BH1750FVI_Send_CMD(uint8_t cmd);
uint16_t BH1750FVI_Read_I2C_Bus();

#endif