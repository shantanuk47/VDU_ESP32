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

// I2C Pins for LCD Display (change if needed)
#define VDU_LCD_I2C_SDA    21
#define VDU_LCD_I2C_SCL    22

// CAN Bus Pins (example, edit as needed)
#define VDU_CAN_TX         5
#define VDU_CAN_RX         4

// Any additional GPIOs for buttons, relays, etc.
#define VDU_BTN1           13
#define VDU_BTN2           14

// Add more pins as required for your hardware

#endif // VDU_PINS_H
