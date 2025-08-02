#ifndef DATE_TIME_H
#define DATE_TIME_H

#include "lib/DS3231/rtc.h"
#include <stdint.h>
#include <stdbool.h>
#include "esp_err.h"

/* Date/Time display formats */
typedef enum {
    DATE_TIME_FORMAT_SHORT = 0,    /* DD/MM HH:MM */
    DATE_TIME_FORMAT_LONG,         /* DD MMM YYYY */
    DATE_TIME_FORMAT_TIME_ONLY,    /* HH:MM:SS */
    DATE_TIME_FORMAT_FULL          /* DD MMM YYYY HH:MM */
} date_time_format_t;

/* Date/Time display structure */
typedef struct {
    rtc_time_t current_time;
    bool time_valid;
    uint32_t last_update;
    date_time_format_t format;
} date_time_display_t;

/* Function declarations */
esp_err_t date_time_init(void);
esp_err_t date_time_update(date_time_display_t *display);
void date_time_format_short(const rtc_time_t *time, char *buffer, size_t buffer_size);
void date_time_format_long(const rtc_time_t *time, char *buffer, size_t buffer_size);
void date_time_format_time_only(const rtc_time_t *time, char *buffer, size_t buffer_size);
void date_time_format_full(const rtc_time_t *time, char *buffer, size_t buffer_size);
void date_time_get_month_name(uint8_t month, char *buffer, size_t buffer_size);
void date_time_get_day_name(uint8_t day_of_week, char *buffer, size_t buffer_size);
bool date_time_is_valid(const rtc_time_t *time);
esp_err_t date_time_set_current_time(date_time_display_t *display);

#endif /* DATE_TIME_H */ 