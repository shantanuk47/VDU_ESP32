/***************************************************************
 * system_util.h
 * System utilities for ESP32: RAM, Flash, Chip info
 * Project: VDU_ESP32
 * Author: Shantanu Kumar
 * Date: 2025-07-31
 ***************************************************************/

#ifndef SYSTEM_UTIL_H
#define SYSTEM_UTIL_H

/* Print RAM (heap) usage */ 
void system_print_ram_info(void);

/* Print Flash storage info */
void system_print_flash_info(void);

/* Print chip/CPU info */
void system_print_chip_info(void);

/* Print Partition Table info */
void system_print_partition_table(void);

/* Print all system info (calls all above) */
void system_print_all_info(void);


#endif /* SYSTEM_UTIL_H */
