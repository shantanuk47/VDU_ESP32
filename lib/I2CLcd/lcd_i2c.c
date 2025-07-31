/***************************************************************
 * lcd_i2c.c
 * Minimal I2C LCD driver for ESP32 (C, ESP-IDF I2C API)
 * Project: VDU_ESP32 (Vehicle Display Unit)
 * Author: Shantanu Kumar
 * Date: 2025-07-31
 ***************************************************************/

#include "lcd_i2c.h"
#include "driver/i2c.h"
#include <string.h>
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

// LCD commands and flags
#define LCD_BACKLIGHT   0x08
#define LCD_ENABLE      0x04
#define LCD_CMD         0
#define LCD_DATA        1

#define I2C_PORT        I2C_NUM_0

static uint8_t lcd_addr = LCD_I2C_ADDR;

static void i2c_master_init(void)
{
    static bool i2c_initialized = false;
    if (i2c_initialized) return;

    i2c_config_t conf = {
        .mode = I2C_MODE_MASTER,
        .sda_io_num = VDU_LCD_I2C_SDA,
        .scl_io_num = VDU_LCD_I2C_SCL,
        .sda_pullup_en = GPIO_PULLUP_ENABLE,
        .scl_pullup_en = GPIO_PULLUP_ENABLE,
        .master.clk_speed = 100000,
    };
    i2c_param_config(I2C_PORT, &conf);
    i2c_driver_install(I2C_PORT, conf.mode, 0, 0, 0);
    i2c_initialized = true;
}

static void lcd_send_byte(uint8_t val, uint8_t mode)
{
    uint8_t data_u = (val & 0xF0) | LCD_BACKLIGHT | mode;
    uint8_t data_l = ((val << 4) & 0xF0) | LCD_BACKLIGHT | mode;
    uint8_t data_arr[4] = {
        data_u | LCD_ENABLE,
        data_u,
        data_l | LCD_ENABLE,
        data_l
    };

    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, (lcd_addr << 1) | I2C_MASTER_WRITE, true);
    i2c_master_write(cmd, data_arr, 4, true);
    i2c_master_stop(cmd);
    i2c_master_cmd_begin(I2C_PORT, cmd, 1000 / portTICK_PERIOD_MS);
    i2c_cmd_link_delete(cmd);
}

static void lcd_write_cmd(uint8_t cmd)
{
    lcd_send_byte(cmd, LCD_CMD);
    vTaskDelay(pdMS_TO_TICKS(2));
}

static void lcd_write_data(uint8_t data)
{
    lcd_send_byte(data, LCD_DATA);
    vTaskDelay(pdMS_TO_TICKS(2));
}

void lcd_i2c_init(void)
{
    i2c_master_init();
    vTaskDelay(pdMS_TO_TICKS(50));
    lcd_write_cmd(0x33); // init
    lcd_write_cmd(0x32); // set to 4-bit mode
    lcd_write_cmd(0x28); // 2 line, 5x7 matrix
    lcd_write_cmd(0x0C); // display ON, cursor off
    lcd_write_cmd(0x06); // move cursor right
    lcd_i2c_clear();
}

void lcd_i2c_clear(void)
{
    lcd_write_cmd(0x01);
    vTaskDelay(pdMS_TO_TICKS(2));
}

void lcd_i2c_set_cursor(uint8_t col, uint8_t row)
{
    uint8_t row_offsets[] = {0x00, 0x40, 0x14, 0x54};
    lcd_write_cmd(0x80 | (col + row_offsets[row]));
}

void lcd_i2c_print(const char *str)
{
    for (int i = 0; str[i]; ++i)
        lcd_write_data(str[i]);
}
