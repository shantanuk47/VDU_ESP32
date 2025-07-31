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
#include "dashboard.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"

void app_main(void)
{
    /* Print all system info (RAM, Flash, Chip, Partitions) at startup */
    system_print_all_info();

    /* Initialize all hardware (pins, LCD, etc.) */
    vdu_pins_init();
    
    /* Initialize LCD first */
    lcd_i2c_init();

    /* Simple LCD test */
    lcd_i2c_clear();
    lcd_i2c_set_cursor(0, 0);
    lcd_i2c_print("VDU Ready");
    lcd_i2c_set_cursor(0, 1);
    lcd_i2c_print("Press BOOT");
    vTaskDelay(pdMS_TO_TICKS(2000)); /* Show for 2 seconds */

    /* Start serial command handler (INFO etc.) */
    serial_init();

    /* Initialize dashboard */
    dashboard_init();

    /* Dashboard data */
    dashboard_data_t dashboard_data = {0};
    unsigned int speed = 0;
    float odo = 12345.0; /* Use float for decimal odometer */
    unsigned long last_odo_update = 0;
    const unsigned long UPDATE_INTERVAL_MS = 100; /* Update every 100ms for smooth increment */

    printf("VDU Dashboard Ready! Press BOOT button to navigate pages.\n");
    printf("Pages: Speed -> Engine -> Fuel -> Trip -> Compact\n");

    while (1)
    {
        /* Update speed and odometer */
        speed = 100 + (speed - 100 + 1) % 21; /* Cycle between 100-120 */
        
        /* Update odometer based on speed */
        unsigned long current_time = xTaskGetTickCount() * portTICK_PERIOD_MS;
        if (current_time - last_odo_update >= UPDATE_INTERVAL_MS) {
            float distance_increment = (speed * UPDATE_INTERVAL_MS) / 3600000.0;
            if (distance_increment > 0) {
                odo += distance_increment;
            }
            last_odo_update = current_time;
        }

        /* Update dashboard data */
        dashboard_data.speed = speed;
        dashboard_data.odometer = odo;
        dashboard_update_data(&dashboard_data);

        /* Check button presses */
        dashboard_check_buttons();

        /* Show current dashboard page */
        dashboard_show_page(dashboard_get_current_page(), &dashboard_data);
        
        /* Fixed delay for stable operation */
        vTaskDelay(pdMS_TO_TICKS(100));
    }
}
