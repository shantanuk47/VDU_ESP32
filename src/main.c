/***************************************************************
 * main.c
 * Main entry for VDU_ESP32, using pure C driver modules.
 *
 * Project: VDU_ESP32 (Vehicle Display Unit)
 * Author: Shantanu Kumar
 * Date: 2025-07-31
 ***************************************************************/

#include <pins.h>
#include "lcd_i2c.h"
#include "vdu_display.h"
#include "system_util.h"
#include "serial.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

void app_main(void)
{
    /* Print all system info (RAM, Flash, Chip, Partitions) at startup */
    system_print_all_info();

    /* Initialize all hardware (pins, LCD, etc.) */
    vdu_pins_init();
    lcd_i2c_init();

    /* Start serial command handler (INFO etc.) */
    serial_init();

    unsigned int speed = 0;
    unsigned long odo = 12345;

    while (1)
    {
        vdu_show_dashboard(speed, odo);
        speed = (speed + 1) % 121;
        odo++;
        vTaskDelay(pdMS_TO_TICKS(500));
    }
}
