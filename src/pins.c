/***************************************************************
 * pins.c
 * Initialization routines for ESP32 pins (if needed)
 * Project: VDU_ESP32 (Vehicle Display Unit)
 * Author: Shantanu Kumar
 * Date: 2025-07-31
 ***************************************************************/

#include "pins.h"
#include "driver/gpio.h"

/* Setup for buttons (INPUT with pullup) */
void vdu_pins_init(void)
{
    gpio_config_t io_conf = {0};
    io_conf.intr_type = GPIO_INTR_DISABLE;
    io_conf.mode = GPIO_MODE_INPUT;
    io_conf.pin_bit_mask = (1ULL << VDU_BTN1) | (1ULL << VDU_BTN2);
    io_conf.pull_up_en = 1;
    gpio_config(&io_conf);

    /* Note: BOOT button (GPIO 0) has built-in pull-up */
    /* External button (GPIO 13) will be configured when available */
}
