#include "tcs34725.h"

static const char *tag = "TCS34725";

static i2c_master_dev_handle_t i2c_master_dev_handle = NULL;

static tcs34725_data_t values;

void TCS34725_Add_To_I2C_Bus()
{
    ESP_LOGI(tag, "I2C device init");
    i2c_device_config_t i2c_dev_cfg = {
        .device_address = I2C_TCS34725_ADDR,
        .dev_addr_length = I2C_ADDR_BIT_LEN_7,
        .scl_speed_hz = I2C_CLK_SPEED,
    };

    ESP_ERROR_CHECK(i2c_master_bus_add_device(i2c_master_bus_handle, &i2c_dev_cfg, &i2c_master_dev_handle));

}

void TCS34725_Send_CMD(uint8_t reg, uint8_t value)
{
    uint8_t cmd[2] = {CMD_BIT | reg, value };
    ESP_ERROR_CHECK(i2c_master_transmit(i2c_master_dev_handle, cmd, sizeof(cmd), -1));
    
}

void TCS34725_Init() 
{
    TCS34725_Add_To_I2C_Bus();
    TCS34725_Send_CMD(ATIME, INTEGRATIONTIME_154MS);
    TCS34725_Send_CMD(CONTROL, GAIN_16X);
    TCS34725_Send_CMD(ENABLE, ENABLE_PON);
    vTaskDelay(pdMS_TO_TICKS(10));

    TCS34725_Send_CMD(ENABLE, ENABLE_PON | ENABLE_AEN);
    vTaskDelay(pdMS_TO_TICKS(160));
}



uint16_t TCS34725_Read_I2C_Bus(uint8_t reg)
{
    uint8_t reg_addr = CMD_BIT | reg;
    uint8_t buffer[2] = {0};
    uint16_t value;

    TCS34725_Send_CMD(reg_addr, 0);

    ESP_ERROR_CHECK(i2c_master_receive(i2c_master_dev_handle, buffer, sizeof(buffer), -1));
    return (value = ((buffer[1] << 8) | buffer[0]));
}

tcs34725_data_t* TCS34725_Get_RGBC_Data() 
{
    values.clear = TCS34725_Read_I2C_Bus(CDATAL + 0);
    values.red = TCS34725_Read_I2C_Bus(CDATAL + 2);
    values.green = TCS34725_Read_I2C_Bus(CDATAL + 4);
    values.blue = TCS34725_Read_I2C_Bus(CDATAL + 6);
    //ESP_LOGI(tag, "Red %d, Green %d, Blue %d, Clear %d", values.red, values.green, values.blue, values.clear);
    return &values;
}

uint16_t TCS34725_RGBC_to_Lux() 
{
    return (uint16_t)((0.026f * values.clear) - (0.086f * values.red) + 
           (0.177f * values.green) - (0.057f * values.blue));
}

uint16_t TCS34725_RGBC_to_Color_Temperature()
{
    TCS34725_Get_RGBC_Data();

    float R = (float)values.red;
    float G = (float)values.green;
    float B = (float)values.blue;

    float X = -0.14282f * R +  1.54924f * G + -0.95641f * B;
    float Y = -0.32466f * R +  1.57837f * G + -0.73191f * B;
    float Z = -0.68202f * R +  0.77073f * G +  0.56332f * B;

    float sum = X + Y + Z;
    if (sum == 0.0f) 
        return 0;

    float xc = X / sum;
    float yc = Y / sum;
    float n  = (xc - 0.3320f) / (0.1858f - yc);
    float color_temperature = 449.0f * powf(n, 3.0f)
              + 3525.0f * powf(n, 2.0f)
              + 6823.3f * n
              + 5520.33f;

    return (uint16_t)roundf(color_temperature);
}

void TCS34725_Color_Temperature_to_RGBC(tcs34725_data_t* rgbc_values, uint16_t color_temperature) 
{
    float T = (float)color_temperature;
    float x, y;

    x = 0.3366f + 0.0002067f * T - 0.000000454f * T * T;
    y = 0.1735f + 0.0002533f * T - 0.000000217f * T * T;

    float Y_val = 1.0f;
    float X = (x / y) * Y_val;
    float Z = ((1.0f - x - y) / y) * Y_val;

    float R = 24.12f * X + 11.31f * Y_val + 13.69f * Z;
    float G = -26.70f * X -12.15f * Y_val -15.69f * Z;
    float B = 6.236f * X + 3.408f * Y_val + 4.60f * Z;

    rgbc_values->red   = (uint16_t)fmaxf(0.0f, R);
    rgbc_values->green = (uint16_t)fmaxf(0.0f, G);
    rgbc_values->blue  = (uint16_t)fmaxf(0.0f, B);
    rgbc_values->clear = rgbc_values->red + rgbc_values->green + rgbc_values->blue;
}
