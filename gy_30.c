#include "gy_30.h"

static const char *tag = "BH1750FVI";

static i2c_master_dev_handle_t i2c_master_dev_handle = NULL;

void BH1750FVI_Add_To_I2C_Bus()
{
    ESP_LOGI(tag, "I2C device init");
    i2c_device_config_t i2c_dev_cfg = {
        .device_address = I2C_BH1750FVI_ADDR,
        .dev_addr_length = I2C_ADDR_BIT_LEN_7,
        .scl_speed_hz = I2C_CLK_SPEED,
    };

    ESP_ERROR_CHECK(i2c_master_bus_add_device(i2c_master_bus_handle, &i2c_dev_cfg, &i2c_master_dev_handle));
}

void BH1750FVI_Init()
{
    BH1750FVI_Add_To_I2C_Bus();
    BH1750FVI_Send_CMD(POWER_ON);
}

void BH1750FVI_Send_CMD(uint8_t cmd)
{
    ESP_ERROR_CHECK(i2c_master_transmit(i2c_master_dev_handle, &cmd, sizeof(cmd), -1));
}

uint16_t BH1750FVI_Read_I2C_Bus()
{
    uint8_t buffer[2];
    uint16_t value;

    BH1750FVI_Send_CMD(CONT_H_MODE);
    vTaskDelay(pdMS_TO_TICKS(160));

    ESP_ERROR_CHECK(i2c_master_receive(i2c_master_dev_handle, buffer, sizeof(buffer), -1));
    return (value = ((buffer[0] << 8) | buffer[1])) / 1.2;
}

