/***************************************************************
 * vdu_display.h
 * Dashboard display logic for VDU_ESP32 in pure C.
 * 
 * Project: VDU_ESP32 (Vehicle Display Unit)
 * Author: Shantanu Kumar
 * Date: 2025-07-31
 ***************************************************************/

#ifndef VDU_DISPLAY_H
#define VDU_DISPLAY_H

#include <stdint.h>

// Show speed and odometer on LCD
void vdu_show_dashboard(unsigned int speed, unsigned long odometer);

// You can add more display functions here, e.g. warnings, menus, faults.

#endif // VDU_DISPLAY_H
