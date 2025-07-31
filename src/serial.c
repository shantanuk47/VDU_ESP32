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

/* Serial command task */
static void serial_command_task(void *pvParameters)
{
    char buf[SERIAL_RX_BUF_SIZE] = {0};
    int idx = 0;
    while (1)
    {
        uint8_t ch;
        int len = uart_read_bytes(SERIAL_PORT_NUM, &ch, 1, 10 / portTICK_PERIOD_MS);
        if (len > 0)
        {
            if (ch == '\n' || ch == '\r')
            {
                buf[idx] = '\0';
                if (idx > 0)
                {
                    /* Command: INFO */
                    if (strcmp(buf, "INFO") == 0)
                    {
                        system_print_all_info();
                    }
                    /* Add more commands here if needed */
                }
                idx = 0;
                buf[0] = 0;
            }
            else if (idx < SERIAL_RX_BUF_SIZE - 1)
            {
                buf[idx++] = ch;
            }
        }
        vTaskDelay(pdMS_TO_TICKS(10));
    }
}

/* Serial/UART initialization and command handler launcher */
void serial_init(void)
{
    uart_driver_install(SERIAL_PORT_NUM, 256, 0, 0, NULL, 0);
    xTaskCreate(serial_command_task, "serial_command", 4096, NULL, 5, NULL);
}
