/***************************************************************
 * pins.h
 * Central configuration for ESP32 pin assignments
 * Project: VDU_ESP32 (Vehicle Display Unit)
 * Author: Shantanu Kumar
 * Date: 2025-07-31
 ***************************************************************/

#ifndef VDU_PINS_H
#define VDU_PINS_H

#include <stdint.h>

/* I2C Pins for LCD Display (change if needed) */
#define VDU_LCD_I2C_SDA    21
#define VDU_LCD_I2C_SCL    22

/* CAN Bus Pins (example, edit as needed) */
#define VDU_CAN_TX         5
#define VDU_CAN_RX         4

/* Button Pins - Using built-in BOOT button temporarily */
#define VDU_BTN1           0   /* Built-in BOOT button (GPIO 0) */
#define VDU_BTN2           13  /* External button (when available) */

/* Add more pins as required for your hardware */
void vdu_pins_init(void);

#endif /* VDU_PINS_H */
