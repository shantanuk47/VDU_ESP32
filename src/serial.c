/***************************************************************
 * serial.c
 * Serial command interface for VDU_ESP32 (ESP32)
 * Author: Shantanu Kumar
 * Date: 2025-07-31
 ***************************************************************/

#include "serial.h"
#include "system_util.h"
#include "driver/uart.h"
#include "lib/DS3231/rtc.h"
#include <string.h>
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#define SERIAL_PORT_NUM      UART_NUM_0
#define SERIAL_RX_BUF_SIZE   128
#define SERIAL_READ_SIZE     16  /* Read multiple characters at once */

/* Command structure for efficient lookup */
typedef struct {
    const char *cmd;
    void (*handler)(void);
} serial_command_t;

/* Command handlers */
static void cmd_info(void) {
    printf("=== VDU ESP32 System Information ===\n");
    printf("Project: VDU_ESP32\n");
    printf("Version: 1.0.0 (Development)\n");
    printf("Hardware: ESP32 DevKit V1\n");
    printf("Display: 16x2 LCD (I2C)\n");
    printf("RTC: DS3231 (I2C)\n");
    printf("CAN: MCP2551 Transceiver\n");
    printf("Status: System Running\n");
    printf("==============================\n");
}

static void cmd_test(void) {
    printf("TEST command received - serial communication is working!\n");
}

static void cmd_set_time(void) {
    /* This will be called when SET_TIME command is received */
    /* The actual parsing and setting will be done in the command processing loop */
}

/* Command lookup table */
static const serial_command_t commands[] = {
    {"INFO", cmd_info},
    {"TEST", cmd_test},
    {"SET_TIME", cmd_set_time},
    /* Add more commands here */
};

#define NUM_COMMANDS (sizeof(commands) / sizeof(commands[0]))

/* Serial command task */
static void serial_command_task(void *pvParameters)
{
    char buf[SERIAL_RX_BUF_SIZE] = {0};
    int idx = 0;
    uint8_t data[SERIAL_READ_SIZE];
    

    
    while (1)
    {
        int len = uart_read_bytes(SERIAL_PORT_NUM, data, SERIAL_READ_SIZE, 50 / portTICK_PERIOD_MS);
        if (len > 0)
        {
            for (int i = 0; i < len; i++) {
                uint8_t ch = data[i];
                
                if (ch == '\n' || ch == '\r' || ch == '"')
                {
                    buf[idx] = '\0';
                    if (idx > 0)
                    {
                        /* Check for SET_TIME command with parameters (handle echo prefix and quotes) */
                        char *set_time_cmd = strstr(buf, "SET_TIME ");
                        if (set_time_cmd != NULL) {
                            /* Remove quotes if present */
                            char *quote_start = strchr(set_time_cmd, '"');
                            if (quote_start) {
                                set_time_cmd = quote_start + 1;
                                char *quote_end = strrchr(set_time_cmd, '"');
                                if (quote_end) {
                                    *quote_end = '\0';
                                }
                            }
                            /* Parse SET_TIME command: SET_TIME YYYY MM DD HH MM SS */
                            int year, month, day, hour, minute, second;
                            if (sscanf(set_time_cmd + 9, "%d %d %d %d %d %d", 
                                     &year, &month, &day, &hour, &minute, &second) == 6) {
                                
                                /* Validate ranges */
                                if (year >= 2020 && year <= 2030 &&
                                    month >= 1 && month <= 12 &&
                                    day >= 1 && day <= 31 &&
                                    hour >= 0 && hour <= 23 &&
                                    minute >= 0 && minute <= 59 &&
                                    second >= 0 && second <= 59) {
                                    
                                    /* Set RTC time */
                                    rtc_time_t new_time = {
                                        .seconds = second,
                                        .minutes = minute,
                                        .hours = hour,
                                        .day_of_week = 1, /* Will be calculated */
                                        .date = day,
                                        .month = month,
                                        .year = year
                                    };
                                    
                                    esp_err_t result = ds3231_set_time(&new_time);
                                    if (result == ESP_OK) {
                                        printf("RTC time set successfully: %04d-%02d-%02d %02d:%02d:%02d\n", 
                                               year, month, day, hour, minute, second);
                                    } else {
                                        printf("Failed to set RTC time: %s\n", esp_err_to_name(result));
                                    }
                                } else {
                                    printf("Invalid time values\n");
                                }
                            } else {
                                printf("Invalid SET_TIME format. Use: SET_TIME YYYY MM DD HH MM SS\n");
                            }
                        } else {
                            /* Look up other commands in table */
                            for (int j = 0; j < NUM_COMMANDS; j++) {
                                if (strcmp(buf, commands[j].cmd) == 0) {
                                    commands[j].handler();
                                    break;
                                }
                            }
                        }
                    }
                    idx = 0;
                    buf[0] = 0;
                }
                else if (idx < SERIAL_RX_BUF_SIZE - 1)
                {
                    buf[idx++] = ch;
                }
            }
        }
        vTaskDelay(pdMS_TO_TICKS(5)); /* Reduced from 10ms to 5ms */
    }
}

/* Serial/UART initialization and command handler launcher */
void serial_init(void)
{
    uart_driver_install(SERIAL_PORT_NUM, 256, 0, 0, NULL, 0);
    xTaskCreate(serial_command_task, "serial_command", 4096, NULL, 5, NULL);
}
