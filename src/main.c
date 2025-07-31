/***************************************************************
 * main.c
 * Main entry for VDU_ESP32, using pure C driver modules.
 * 
 * Project: VDU_ESP32 (Vehicle Display Unit)
 * Author: Shantanu Kumar
 * Date: 2025-07-31
 ***************************************************************/

#include "config/pins.h"
#include "lcd_i2c.h"
#include "vdu_display.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

void app_main(void)
{
    vdu_pins_init();      // If you have pin init routines (optional)
    lcd_i2c_init();

    unsigned int speed = 0;
    unsigned long odo = 12345;

    while (1)
    {
        vdu_show_dashboard(speed, odo);
        speed = (speed + 1) % 121;    // Simulate speed up to 120 KMPH
        odo++;                        // Simulate ODO increment
        vTaskDelay(pdMS_TO_TICKS(500));
    }
}
