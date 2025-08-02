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
#include "ver.h"
#include "can_comm.h"
#include "lib/DS3231/rtc.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"

#include "esp_log.h"
void app_main(void)
{
    /* Print version information at startup */
    print_version_info();
    
    /* Print all system info (RAM, Flash, Chip, Partitions) at startup */
    system_print_all_info();

    /* Initialize all hardware (pins, LCD, etc.) */
    vdu_pins_init();
    
    /* Initialize LCD first */
    lcd_i2c_init();
    
    /* Wait for I2C driver to be fully ready */
    vTaskDelay(pdMS_TO_TICKS(100));

    /* Simple LCD test with smart version display */
    lcd_i2c_clear();
    lcd_i2c_set_cursor(0, 0);
    lcd_i2c_print("VDU Ready");
    lcd_i2c_set_cursor(0, 1);
    char version_buf[16];
    get_smart_version_display_string(version_buf, sizeof(version_buf));
    lcd_i2c_print(version_buf);
    vTaskDelay(pdMS_TO_TICKS(2000)); /* Show for 2 seconds */

    /* Start serial command handler (INFO etc.) */
    serial_init();

    /* Initialize CAN communication */
    can_comm_status_t can_status = can_comm_init();
    if (can_status != CAN_COMM_OK) {
        ESP_LOGE("MAIN", "Failed to initialize CAN communication: %d", can_status);
    } else {
        ESP_LOGI("MAIN", "CAN communication initialized successfully");
    }

    /* Initialize RTC (bypass init to avoid I2C conflicts) */
    /* RTC will be available via serial commands only */

    /* Initialize dashboard */
    dashboard_init();

    /* Dashboard data */
    dashboard_data_t dashboard_data = {0};
    unsigned int speed = 0;
    float odo = 12345.0; /* Use float for decimal odometer */
    unsigned long last_odo_update = 0;
    const unsigned long UPDATE_INTERVAL_MS = 500; /* Update every 500ms for smooth increment */
    
    /* Multi-rate timing system */
    unsigned long last_can_send = 0;
    const unsigned long CAN_SEND_INTERVAL_MS = 100; /* CAN: 10 Hz (100ms) - Real-time vehicle data */
    
    unsigned long last_display_update = 0;
    const unsigned long DISPLAY_UPDATE_INTERVAL_MS = 500; /* Display: 2 Hz (500ms) - Good balance, stable but responsive */

    printf("VDU Dashboard Ready! Press BOOT button to navigate pages.\n");
    printf("Pages: Speed -> Engine -> Fuel -> Trip -> Compact\n");

    while (1)
    {
        /* Get current time for multi-rate timing */
        unsigned long current_time = xTaskGetTickCount() * portTICK_PERIOD_MS;
        
        /* Update speed and odometer */
        speed = 100 + (speed - 100 + 1) % 21; /* Cycle between 100-120 */
        
        /* Update odometer based on speed (500ms) */
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

        /* Send CAN message every 100ms (10 Hz) */
        if (current_time - last_can_send >= CAN_SEND_INTERVAL_MS) {
            /* Prepare vehicle data */
            vehicle_data_t vehicle_data = {
                .speed = speed,
                .rpm = 2000 + (speed * 20),
                .temperature = 85 + (speed / 10),
                .fuel_level = 75,
                .engine_running = true,
                .check_engine = false,
                .low_fuel = false,
                .door_open = false
            };
            
            /* Send vehicle data via CAN */
            can_comm_status_t send_status = can_comm_send_vehicle_data(&vehicle_data);
            if (send_status != CAN_COMM_OK) {
                ESP_LOGW("MAIN", "Failed to send CAN message: %d", send_status);
            }
            
            last_can_send = current_time;
        }

        /* Check button presses (10ms - 100 Hz) */
        dashboard_check_buttons();

        /* Process received CAN messages (non-blocking) */
        can_rx_msg_t rx_msg;
        if (can_comm_get_received_message(&rx_msg, 0) == CAN_COMM_OK) {
            /* Here you can add specific handling for different CAN IDs */
            switch (rx_msg.id) {
                case 0x100: /* Engine data */
                    /* Update dashboard with engine data */
                    break;
                case 0x200: /* Vehicle status */
                    /* Update dashboard with vehicle status */
                    break;
                default:
                    /* Unknown CAN ID - just log it */
                    break;
            }
        }

        /* Update display every 500ms (2 Hz) */
        if (current_time - last_display_update >= DISPLAY_UPDATE_INTERVAL_MS) {
            dashboard_show_page(dashboard_get_current_page(), &dashboard_data);
            last_display_update = current_time;
        }
        
        /* System loop delay - 10ms (100 Hz) for responsive button handling */
        vTaskDelay(pdMS_TO_TICKS(10));
    }
}

