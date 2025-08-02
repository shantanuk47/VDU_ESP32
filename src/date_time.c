/***************************************************************
 * date_time.c
 * Date/Time display module for VDU_ESP32
 * 
 * Project: VDU_ESP32 (Vehicle Display Unit)
 * Author: Shantanu Kumar
 * Date: 2025-01-15
 ***************************************************************/

#include "date_time.h"
#include "esp_log.h"
#include "esp_timer.h"
#include <string.h>
#include <stdio.h>

static const char *TAG = "DATE_TIME";

/* Month names */
static const char *month_names[] = {
    "JAN", "FEB", "MAR", "APR", "MAY", "JUN",
    "JUL", "AUG", "SEP", "OCT", "NOV", "DEC"
};

/* Day names */
static const char *day_names[] = {
    "SUN", "MON", "TUE", "WED", "THU", "FRI", "SAT"
};

/* Initialize date/time module */
esp_err_t date_time_init(void) {
    ESP_LOGI(TAG, "Date/Time module initialized");
    return ESP_OK;
}

/* Update date/time display */
esp_err_t date_time_update(date_time_display_t *display) {
    esp_err_t ret = ds3231_get_time(&display->current_time);
    if (ret == ESP_OK) {
        display->time_valid = date_time_is_valid(&display->current_time);
        display->last_update = esp_timer_get_time() / 1000000; /* Convert to seconds */
    } else {
        display->time_valid = false;
        ESP_LOGW(TAG, "Failed to update time: %s", esp_err_to_name(ret));
    }
    return ret;
}

/* Format time in short format (DD/MM HH:MM) */
void date_time_format_short(const rtc_time_t *time, char *buffer, size_t buffer_size) {
    snprintf(buffer, buffer_size, "%02u/%02u %02u:%02u",
             time->date, time->month, time->hours, time->minutes);
}

/* Format time in long format (DD MMM YYYY) */
void date_time_format_long(const rtc_time_t *time, char *buffer, size_t buffer_size) {
    char month_name[4];
    date_time_get_month_name(time->month, month_name, sizeof(month_name));
    snprintf(buffer, buffer_size, "%02u %s %04u",
             time->date, month_name, time->year);
}

/* Format time only (HH:MM:SS) */
void date_time_format_time_only(const rtc_time_t *time, char *buffer, size_t buffer_size) {
    snprintf(buffer, buffer_size, "%02u:%02u:%02u",
             time->hours, time->minutes, time->seconds);
}

/* Format full date/time (DD MMM YYYY HH:MM) */
void date_time_format_full(const rtc_time_t *time, char *buffer, size_t buffer_size) {
    char month_name[4];
    date_time_get_month_name(time->month, month_name, sizeof(month_name));
    snprintf(buffer, buffer_size, "%02u %s %04u %02u:%02u",
             time->date, month_name, time->year, time->hours, time->minutes);
}

/* Get month name */
void date_time_get_month_name(uint8_t month, char *buffer, size_t buffer_size) {
    if (month >= 1 && month <= 12) {
        strncpy(buffer, month_names[month - 1], buffer_size - 1);
        buffer[buffer_size - 1] = '\0';
    } else {
        strncpy(buffer, "???", buffer_size - 1);
        buffer[buffer_size - 1] = '\0';
    }
}

/* Get day name */
void date_time_get_day_name(uint8_t day_of_week, char *buffer, size_t buffer_size) {
    if (day_of_week >= 1 && day_of_week <= 7) {
        strncpy(buffer, day_names[day_of_week - 1], buffer_size - 1);
        buffer[buffer_size - 1] = '\0';
    } else {
        strncpy(buffer, "???", buffer_size - 1);
        buffer[buffer_size - 1] = '\0';
    }
}

/* Check if time is valid */
bool date_time_is_valid(const rtc_time_t *time) {
    /* Basic range checks */
    if (time->seconds > 59 || time->minutes > 59 || time->hours > 23) {
        return false;
    }
    if (time->date < 1 || time->date > 31 || time->month < 1 || time->month > 12) {
        return false;
    }
    if (time->year < 2000 || time->year > 2099) {
        return false;
    }
    if (time->day_of_week < 1 || time->day_of_week > 7) {
        return false;
    }
    
    /* Check for oscillator stop flag */
    if (ds3231_is_oscillator_stopped()) {
        return false;
    }
    
    return true;
}

/* Set current time */
esp_err_t date_time_set_current_time(date_time_display_t *display) {
    esp_err_t ret = ds3231_set_time(&display->current_time);
    if (ret == ESP_OK) {
        display->time_valid = true;
        display->last_update = esp_timer_get_time() / 1000000;
        ESP_LOGI(TAG, "Current time set successfully");
    } else {
        ESP_LOGE(TAG, "Failed to set current time: %s", esp_err_to_name(ret));
    }
    return ret;
} 