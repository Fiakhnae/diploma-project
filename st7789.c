#include "st7789.h"

esp_lcd_spi_bus_handle_t spi_bus_handle = SPI_HOST_NUM;
esp_lcd_panel_io_handle_t st7789_panel_io_handle = NULL;
esp_lcd_panel_handle_t st7789_panel_handle = NULL;

static lv_obj_t * screen = NULL;

void* disp_buf_1 = NULL;
void* disp_buf_2 = NULL;

lv_display_t * disp = NULL;

static const char * tag = "ST7789";

esp_err_t ST7789_SPI_Init()
{
    ESP_LOGI(tag, "SPI bus initialization");

    spi_bus_config_t spi_bus_config = {
        .sclk_io_num = VSPI_CLK_PIN,
        .mosi_io_num = VSPI_MOSI_PIN,
        .miso_io_num = VSPI_MISO_PIN,
        .quadhd_io_num = -1,
        .quadwp_io_num = -1,
        .max_transfer_sz = ST7789_DISPLAY_HEIGHT * ST7789_DISPLAY_WIDTH * 2 + 8,
    };

    return spi_bus_initialize(SPI_HOST_NUM, &spi_bus_config, SPI_DMA_CH_AUTO);
}

esp_err_t ST7789_Panel_IO_Handle_Init()
{
    ESP_LOGI(tag, "SPI panel io initialization");
    esp_lcd_panel_io_spi_config_t st7789_panel_config = {
        .cs_gpio_num = VSPI_CS_PIN,
        .dc_gpio_num = ST7789_DC_PIN,
        .pclk_hz = SPI_CLK_SPEED,
        .spi_mode = 0,
        .lcd_cmd_bits = 8,
        .lcd_param_bits = 8,
        .trans_queue_depth = 10,
    };

    return esp_lcd_new_panel_io_spi(spi_bus_handle, &st7789_panel_config, &st7789_panel_io_handle);
}

esp_err_t ST7789_Panel_Handle_Init()
{
    ESP_LOGI(tag, "Display panel initialization");
    esp_lcd_panel_dev_config_t st7789_panel_dev_config = {
        .bits_per_pixel = ST7789_BITS_PER_PIXEL,
        .reset_gpio_num = ST7789_RST_PIN,
        .color_space = ESP_LCD_COLOR_SPACE_RGB,
    };

    return esp_lcd_new_panel_st7789(st7789_panel_io_handle, &st7789_panel_dev_config, &st7789_panel_handle);
}

void ST7789_Init()
{
    ESP_LOGI(tag, "Initialization");
    ESP_ERROR_CHECK(ST7789_SPI_Init());
    ESP_ERROR_CHECK(ST7789_Panel_IO_Handle_Init());
    ESP_ERROR_CHECK(ST7789_Panel_Handle_Init());

    ESP_LOGI(tag, "Driver configuration");
    ESP_ERROR_CHECK(esp_lcd_panel_reset(st7789_panel_handle));
    ESP_ERROR_CHECK(esp_lcd_panel_init(st7789_panel_handle));
    ESP_ERROR_CHECK(esp_lcd_panel_invert_color(st7789_panel_handle, false));
    ESP_ERROR_CHECK(esp_lcd_panel_mirror(st7789_panel_handle, true, false));
    ESP_ERROR_CHECK(esp_lcd_panel_swap_xy(st7789_panel_handle, true));
    ESP_ERROR_CHECK(esp_lcd_panel_set_gap(st7789_panel_handle, 1, 2));
    ESP_ERROR_CHECK(esp_lcd_panel_disp_on_off(st7789_panel_handle, true));

}

void my_flush_cb(lv_display_t * display, const lv_area_t * area, uint8_t * px_map)
{
    //ESP_LOGI(tag, "Flushing area: (%ld,%ld)-(%ld,%ld)", area->x1, area->x2, area->y1, area->y2);
    esp_lcd_panel_draw_bitmap(st7789_panel_handle, area->x1, area->y1, area->x2 + 1, area->y2 + 1, px_map);
    lv_display_flush_ready(display);
}

void ST7789_LVGL_Init()
{
    ESP_LOGI(tag, "LVGL initialization");
    lv_init();

    ESP_LOGI(tag, "Creating display");
    disp = lv_display_create(ST7789_DISPLAY_HEIGHT, ST7789_DISPLAY_WIDTH);

    ESP_LOGI(tag, "Setting buffers");
    const uint16_t disp_bufsize = ST7789_DISPLAY_WIDTH * ST7789_DISPLAY_HEIGHT / 8;
    disp_buf_1 = heap_caps_calloc(disp_bufsize, sizeof(uint16_t), MALLOC_CAP_DMA);
    disp_buf_2 = heap_caps_calloc(disp_bufsize, sizeof(uint16_t), MALLOC_CAP_DMA);

    ESP_LOGI(tag, "Display configuration");
    lv_display_set_antialiasing(disp, true);
    lv_display_set_color_format(disp, LV_COLOR_FORMAT_RGB565);
    lv_display_set_resolution(disp, ST7789_DISPLAY_HEIGHT, ST7789_DISPLAY_WIDTH);
    lv_display_set_rotation(disp, LV_DISPLAY_ROTATION_0);
    lv_display_set_buffers(disp, disp_buf_1, disp_buf_2, disp_bufsize * sizeof(uint16_t), LV_DISPLAY_RENDER_MODE_PARTIAL);
    lv_display_set_flush_cb(disp, my_flush_cb);
    lv_display_set_default(disp);
    //lv_display_set_offset(disp, 1, 2);
}

void ST7789_Create_Label(lv_obj_t **label, lv_align_t align, int32_t x_ofs, int32_t y_ofs)
{
    screen = lv_screen_active();
    *label = lv_label_create(screen);

    lv_obj_set_style_bg_color(screen, lv_color_hex(0x000000), LV_PART_MAIN);
    lv_obj_set_style_text_font(*label, &lv_font_montserrat_14, LV_PART_MAIN);
    lv_obj_set_style_text_color(*label, lv_color_hex(0xffffff), LV_PART_MAIN);

    lv_obj_align(*label, align, x_ofs, y_ofs);
}

void ST7789_Show_Text(lv_obj_t *label, const char * user_text, uint16_t value)
{
    lv_lock();
    //ESP_LOGI(tag, "Setting text on display");
    lv_label_set_text_fmt(label, user_text, value);
    lv_refr_now(NULL);
    //ESP_LOGI(tag, "Text: %s", lv_label_get_text(text));
    lv_unlock();
}

void lvgl_task(void *pvParam) 
{
    while (1) 
    {
        lv_timer_handler();
        //ESP_LOGI(tag, "lvgl_task running");
        vTaskDelay(pdMS_TO_TICKS(10)); // Виклик кожні 10 мс
    }
}

