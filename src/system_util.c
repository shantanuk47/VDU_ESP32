/***************************************************************
 * system_util.c
 * System utilities for ESP32: RAM, Flash, Chip info, Partition Table
 * Project: VDU_ESP32
 * Author: Shantanu Kumar
 * Date: 2025-07-31
 ***************************************************************/

#include "system_util.h"
#include <stdio.h>
#include <inttypes.h>
#include "esp_heap_caps.h"
#include "esp_system.h"
#include "esp_flash.h"
#include "esp_chip_info.h"
#include "esp_partition.h"

/* RAM (heap) info */
void system_print_ram_info(void)
{
    size_t total = heap_caps_get_total_size(MALLOC_CAP_DEFAULT);
    size_t free  = heap_caps_get_free_size(MALLOC_CAP_DEFAULT);
    size_t used  = total - free;
    printf("RAM: %u/%u bytes (%.1f%% used)\n", 
           (unsigned int)used, (unsigned int)total, 
           (float)used * 100.0f / total);
}

/* Flash info (Total, Used, Free) */
void system_print_flash_info(void)
{
    extern esp_flash_t *esp_flash_default_chip;
    uint32_t total = 0;
    esp_flash_get_size(esp_flash_default_chip, &total);

    /* Get app (firmware) partition size */
    const esp_partition_t *app_part = esp_partition_find_first(
        ESP_PARTITION_TYPE_APP, ESP_PARTITION_SUBTYPE_ANY, NULL);

    uint32_t used = 0;
    if (app_part) {
        used += app_part->size;
    }

    /* Add NVS partition size to "used" if present */
    const esp_partition_t *nvs_part = esp_partition_find_first(
        ESP_PARTITION_TYPE_DATA, ESP_PARTITION_SUBTYPE_DATA_NVS, NULL);
    if (nvs_part) {
        used += nvs_part->size;
    }

    printf("Flash: %u/%u bytes (%.1f%% used)\n", 
           (unsigned int)used, (unsigned int)total,
           (float)used * 100.0f / total);
}

/* Chip/CPU info */
void system_print_chip_info(void)
{
    esp_chip_info_t chip_info;
    esp_chip_info(&chip_info);
    printf("ESP32: %d cores, rev %d", chip_info.cores, chip_info.revision);
    
    if (chip_info.features & CHIP_FEATURE_WIFI_BGN) printf(", WiFi");
    if (chip_info.features & CHIP_FEATURE_BLE) printf(", BLE");
    if (chip_info.features & CHIP_FEATURE_BT) printf(", BT");
    printf("\n");
}

/* Partition table (simplified) */
void system_print_partition_table(void)
{
    printf("Partitions:\n");
    
    /* Print APP partitions */
    esp_partition_iterator_t it = esp_partition_find(ESP_PARTITION_TYPE_APP, ESP_PARTITION_SUBTYPE_ANY, NULL);
    while (it != NULL) {
        const esp_partition_t* p = esp_partition_get(it);
        printf("  %-8s: %u bytes @ 0x%x\n", p->label, (unsigned int)p->size, (unsigned int)p->address);
        it = esp_partition_next(it);
    }
    esp_partition_iterator_release(it);
    
    /* Print DATA partitions */
    it = esp_partition_find(ESP_PARTITION_TYPE_DATA, ESP_PARTITION_SUBTYPE_ANY, NULL);
    while (it != NULL) {
        const esp_partition_t* p = esp_partition_get(it);
        printf("  %-8s: %u bytes @ 0x%x\n", p->label, (unsigned int)p->size, (unsigned int)p->address);
        it = esp_partition_next(it);
    }
    esp_partition_iterator_release(it);
}

/* Print all info at once */
void system_print_all_info(void)
{
    printf("\n=== VDU_ESP32 System Info ===\n");
    system_print_ram_info();
    system_print_flash_info();
    system_print_chip_info();
    system_print_partition_table();
    printf("============================\n");
}
