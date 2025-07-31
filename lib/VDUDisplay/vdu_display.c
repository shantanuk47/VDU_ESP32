/***************************************************************
 * vdu_display.c
 * Dashboard display logic for VDU_ESP32 in pure C.
 * 
 * Project: VDU_ESP32 (Vehicle Display Unit)
 * Author: Shantanu Kumar
 * Date: 2025-07-31
 ***************************************************************/

#include "vdu_display.h"
#include "lcd_i2c.h"
#include <stdio.h>

// Show the current speed and odometer value on the LCD
void vdu_show_dashboard(unsigned int speed, unsigned long odometer)
{
    char buf[17];

    // Line 1: Speed
    lcd_i2c_set_cursor(0, 0);
    snprintf(buf, sizeof(buf), "SPD:%3u KMPH    ", speed);
    lcd_i2c_print(buf);

    // Line 2: Odometer
    lcd_i2c_set_cursor(0, 1);
    snprintf(buf, sizeof(buf), "ODO:%06lu KM   ", odometer);
    lcd_i2c_print(buf);
}
