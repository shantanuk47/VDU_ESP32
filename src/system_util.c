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
    printf("\n=== RAM Usage ===\n");
    printf("  Total: %u bytes\n", (unsigned int)total);
    printf("  Used:  %u bytes\n", (unsigned int)used);
    printf("  Free:  %u bytes\n", (unsigned int)free);
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

    /* Free = total - used */
    uint32_t free = (total > used) ? (total - used) : 0;

    printf("\n=== Flash Storage ===\n");
    printf("  Total: %u bytes\n", (unsigned int)total);
    printf("  Used:  %u bytes\n", (unsigned int)used);
    printf("  Free:  %u bytes\n", (unsigned int)free);
}

/* Chip/CPU info */
void system_print_chip_info(void)
{
    esp_chip_info_t chip_info;
    esp_chip_info(&chip_info);
    printf("\n=== ESP32 Chip Info ===\n");
    printf("  Model:    ESP32\n");
    printf("  Cores:    %d\n", chip_info.cores);
    printf("  Revision: %d\n", chip_info.revision);
    printf("  Features:%s%s%s\n",
           (chip_info.features & CHIP_FEATURE_WIFI_BGN) ? " WiFi" : "",
           (chip_info.features & CHIP_FEATURE_BLE) ? " BLE" : "",
           (chip_info.features & CHIP_FEATURE_BT) ? " BT" : "");
}

/* Partition table (detailed, warning-free) */
void system_print_partition_table(void)
{
    printf("\n=== Flash Partition Table ===\n");
    printf(" Name         | Type | Subtype |  Offset (hex) |   Size (hex) |  Offset (B) |  Size (B)\n");
    printf("----------------------------------------------------------------------------------------\n");

    /* Print all APP partitions */
    esp_partition_iterator_t it = esp_partition_find(ESP_PARTITION_TYPE_APP, ESP_PARTITION_SUBTYPE_ANY, NULL);
    while (it != NULL) {
        const esp_partition_t* p = esp_partition_get(it);
        printf("%-13s| APP  | 0x%02" PRIx32 "   | 0x%08" PRIx32 "   | 0x%08" PRIx32 "  | %10" PRIu32 " | %8" PRIu32 "\n",
               p->label,
               (uint32_t)p->subtype,
               (uint32_t)p->address,
               (uint32_t)p->size,
               (uint32_t)p->address,
               (uint32_t)p->size);
        it = esp_partition_next(it);
    }
    /* Print all DATA partitions */
    it = esp_partition_find(ESP_PARTITION_TYPE_DATA, ESP_PARTITION_SUBTYPE_ANY, NULL);
    while (it != NULL) {
        const esp_partition_t* p = esp_partition_get(it);
        printf("%-13s| DATA | 0x%02" PRIx32 "   | 0x%08" PRIx32 "   | 0x%08" PRIx32 "  | %10" PRIu32 " | %8" PRIu32 "\n",
               p->label,
               (uint32_t)p->subtype,
               (uint32_t)p->address,
               (uint32_t)p->size,
               (uint32_t)p->address,
               (uint32_t)p->size);
        it = esp_partition_next(it);
    }
    printf("----------------------------------------------------------------------------------------\n");
}

/* Print all info at once */
void system_print_all_info(void)
{
    printf("\n============================\n");
    system_print_ram_info();
    system_print_flash_info();
    system_print_chip_info();
    system_print_partition_table();
    printf("============================\n");
}
