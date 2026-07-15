#ifndef ST7789_H
#define ST7789_H

#include "driver/spi_master.h"
#include "esp_lcd_panel_io.h"
#include "esp_lcd_panel_ops.h"
#include "esp_lcd_panel_vendor.h"

#include "esp_heap_caps.h"
#include "esp_log.h"

#include "lvgl.h"

#define SPI_CLK_SPEED SPI_MASTER_FREQ_20M

#define VSPI_CS_PIN 5
#define VSPI_CLK_PIN 18
#define VSPI_MOSI_PIN 23
#define VSPI_MISO_PIN -1

#define SPI_HOST_NUM SPI3_HOST

#define ST7789_DC_PIN 2
#define ST7789_RST_PIN 4

#define ST7789_DISPLAY_HEIGHT 160
#define ST7789_DISPLAY_WIDTH 128

#define ST7789_BITS_PER_PIXEL (8 * 2)

esp_err_t ST7789_SPI_Init();
esp_err_t ST7789_Panel_IO_Handle_Init();
esp_err_t ST7789_Panel_Handle_Init();
void my_flush_cb(lv_display_t * display, const lv_area_t * area, uint8_t * px_map);
void ST7789_Init();
void ST7789_LVGL_Init();
void ST7789_Create_Label(lv_obj_t **label, lv_align_t align, int32_t x_ofs, int32_t y_ofs);
void ST7789_Show_Text(lv_obj_t *label, const char * user_text, uint16_t value);

void lvgl_task(void *pvParam);

#endif