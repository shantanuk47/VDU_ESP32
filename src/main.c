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
#include "can.h"
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
    can_status_t can_status = can_init_default();
    if (can_status != CAN_OK) {
        ESP_LOGE("MAIN", "Failed to initialize CAN communication: %d", can_status);
    } else {
        ESP_LOGI("MAIN", "CAN communication initialized successfully");
    }

    /* Initialize dashboard */
    dashboard_init();

    /* Dashboard data */
    dashboard_data_t dashboard_data = {0};
    unsigned int speed = 0;
    float odo = 12345.0; /* Use float for decimal odometer */
    unsigned long last_odo_update = 0;
    const unsigned long UPDATE_INTERVAL_MS = 100; /* Update every 100ms for smooth increment */
    
    /* CAN message timing */
    unsigned long last_can_send = 0;
    const unsigned long CAN_SEND_INTERVAL_MS = 100; /* Send CAN message every 100ms */

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

        /* Send CAN message every 100ms */
        if (current_time - last_can_send >= CAN_SEND_INTERVAL_MS) {
            /* Prepare CAN message data */
            uint8_t can_data[8] = {0};
            can_data[0] = speed;                                    /* Speed */
            can_data[1] = (2000 + (speed * 20)) >> 8;              /* RPM high byte */
            can_data[2] = (2000 + (speed * 20)) & 0xFF;            /* RPM low byte */
            can_data[3] = 85 + (speed / 10);                       /* Temperature */
            can_data[4] = 75;                                       /* Fuel level */
            can_data[5] = 0x01;                                     /* Status bits (engine running) */
            can_data[6] = can_data[0] ^ can_data[1] ^ can_data[2] ^ can_data[3] ^ can_data[4] ^ can_data[5]; /* Checksum */
            can_data[7] = 0x00;                                     /* Reserved */
            
            /* Prepare CAN message */
            can_message_t can_msg = {
                .id = 0x301,
                .length = 8,
                .is_extended = false,
                .is_remote = false
            };
            
            /* Copy data to CAN message */
            for (int i = 0; i < 8; i++) {
                can_msg.data[i] = can_data[i];
            }
            
            /* Send CAN message */
            can_status_t send_status = can_send(&can_msg);
            if (send_status != CAN_OK) {
                ESP_LOGW("MAIN", "Failed to send CAN message: %d", send_status);
            }
            
            last_can_send = current_time;
        }

        /* Check button presses */
        dashboard_check_buttons();

        /* Show current dashboard page */
        dashboard_show_page(dashboard_get_current_page(), &dashboard_data);
        
        /* Fixed delay for stable operation */
        vTaskDelay(pdMS_TO_TICKS(100));
    }
}

