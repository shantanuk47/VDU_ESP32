/***************************************************************
 * serial.c
 * Serial command interface for VDU_ESP32 (ESP32)
 * Author: Shantanu Kumar
 * Date: 2025-07-31
 ***************************************************************/

#include "serial.h"
#include "system_util.h"
#include "driver/uart.h"
#include <string.h>
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
    system_print_all_info();
}

/* Command lookup table */
static const serial_command_t commands[] = {
    {"INFO", cmd_info},
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
                
                if (ch == '\n' || ch == '\r')
                {
                    buf[idx] = '\0';
                    if (idx > 0)
                    {
                        /* Look up command in table */
                        for (int j = 0; j < NUM_COMMANDS; j++) {
                            if (strcmp(buf, commands[j].cmd) == 0) {
                                commands[j].handler();
                                break;
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
