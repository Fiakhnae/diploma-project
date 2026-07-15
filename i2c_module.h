#ifndef I2C_MODULE_H
#define I2C_MODULE_H

#include "esp_log.h"
#include "driver/i2c_master.h"

#define I2C_PORT_NUM 0
#define I2C_PIN_SCL 22
#define I2C_PIN_SDA 21
#define I2C_CLK_SPEED 400000

extern i2c_master_bus_handle_t i2c_master_bus_handle;

void I2C_Bus_Init();

#endif