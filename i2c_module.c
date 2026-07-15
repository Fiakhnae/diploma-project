#include "i2c_module.h"

i2c_master_bus_handle_t i2c_master_bus_handle = NULL;

static const char *tag = "I2C bus";

void I2C_Bus_Init()
{
    ESP_LOGI(tag, "I2C master bus init");
    i2c_master_bus_config_t i2c_master_bus_cfg = {
        .clk_source = I2C_CLK_SRC_DEFAULT,
        .i2c_port = I2C_PORT_NUM,
        .scl_io_num = I2C_PIN_SCL,
        .sda_io_num = I2C_PIN_SDA,
        .glitch_ignore_cnt = 7,
        .intr_priority = 0,
    };
    ESP_ERROR_CHECK(i2c_new_master_bus(&i2c_master_bus_cfg, &i2c_master_bus_handle));
}