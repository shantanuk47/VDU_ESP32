/***************************************************************
 * lcd_i2c.h
 * Minimal I2C LCD driver for ESP32 (C, ESP-IDF I2C API)
 * Project: VDU_ESP32 (Vehicle Display Unit)
 * Author: Shantanu Kumar
 * Date: 2025-07-31
 ***************************************************************/

#ifndef LCD_I2C_H
#define LCD_I2C_H

#include <stdint.h>

// Use your actual I2C pin config from pins.h!
#include "config/pins.h"

// LCD hardware config
#define LCD_I2C_ADDR      0x27   // Use 0x3F if that's your LCD address
#define LCD_COLS          16
#define LCD_ROWS          2

void lcd_i2c_init(void);
void lcd_i2c_clear(void);
void lcd_i2c_set_cursor(uint8_t col, uint8_t row);
void lcd_i2c_print(const char *str);

#endif // LCD_I2C_H
