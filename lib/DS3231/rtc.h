#ifndef RTC_H
#define RTC_H

#include <stdint.h>
#include <stdbool.h>
#include <time.h>
#include "esp_err.h"

/* DS3231 Register Addresses */
#define DS3231_REG_SECONDS     0x00
#define DS3231_REG_MINUTES     0x01
#define DS3231_REG_HOURS       0x02
#define DS3231_REG_DAY         0x03
#define DS3231_REG_DATE        0x04
#define DS3231_REG_MONTH       0x05
#define DS3231_REG_YEAR        0x06
#define DS3231_REG_ALARM1_SEC  0x07
#define DS3231_REG_ALARM1_MIN  0x08
#define DS3231_REG_ALARM1_HOUR 0x09
#define DS3231_REG_ALARM1_DAY  0x0A
#define DS3231_REG_ALARM2_MIN  0x0B
#define DS3231_REG_ALARM2_HOUR 0x0C
#define DS3231_REG_ALARM2_DAY  0x0D
#define DS3231_REG_CONTROL     0x0E
#define DS3231_REG_STATUS      0x0F
#define DS3231_REG_AGING       0x10
#define DS3231_REG_TEMP_MSB    0x11
#define DS3231_REG_TEMP_LSB    0x12

/* Control Register Bits */
#define DS3231_CONTROL_A1IE    0x01    /* Alarm 1 Interrupt Enable */
#define DS3231_CONTROL_A2IE    0x02    /* Alarm 2 Interrupt Enable */
#define DS3231_CONTROL_INTCN   0x04    /* Interrupt Control */
#define DS3231_CONTROL_RS1     0x08    /* Rate Select 1 */
#define DS3231_CONTROL_RS2     0x10    /* Rate Select 2 */
#define DS3231_CONTROL_CONV    0x20    /* Convert Temperature */
#define DS3231_CONTROL_BBSQW   0x40    /* Battery-Backed Square Wave Enable */
#define DS3231_CONTROL_EOSC    0x80    /* Enable Oscillator */

/* Status Register Bits */
#define DS3231_STATUS_A1F      0x01    /* Alarm 1 Flag */
#define DS3231_STATUS_A2F      0x02    /* Alarm 2 Flag */
#define DS3231_STATUS_BSY      0x04    /* Busy */
#define DS3231_STATUS_EN32KHZ  0x08    /* Enable 32kHz Output */
#define DS3231_STATUS_OSF      0x80    /* Oscillator Stop Flag */

/* Time structure */
typedef struct {
    uint8_t seconds;
    uint8_t minutes;
    uint8_t hours;
    uint8_t day_of_week;
    uint8_t date;
    uint8_t month;
    uint16_t year;
} rtc_time_t;

/* Temperature structure */
typedef struct {
    int16_t temperature;       /* Temperature in 0.25°C units */
    float temperature_celsius; /* Temperature in Celsius */
} rtc_temperature_t;

/* Alarm structure */
typedef struct {
    uint8_t seconds;
    uint8_t minutes;
    uint8_t hours;
    uint8_t day_date;
    bool enabled;
} rtc_alarm_t;

/* Function declarations */
esp_err_t ds3231_init(void);
esp_err_t ds3231_set_time(const rtc_time_t *time);
esp_err_t ds3231_get_time(rtc_time_t *time);
esp_err_t ds3231_set_alarm1(const rtc_alarm_t *alarm);
esp_err_t ds3231_set_alarm2(const rtc_alarm_t *alarm);
esp_err_t ds3231_get_temperature(rtc_temperature_t *temp);
esp_err_t ds3231_clear_alarm_flags(void);
esp_err_t ds3231_get_status(uint8_t *status);
esp_err_t ds3231_set_control(uint8_t control);
esp_err_t ds3231_get_control(uint8_t *control);
bool ds3231_is_oscillator_stopped(void);
esp_err_t ds3231_force_temperature_conversion(void);

/* Utility functions */
void rtc_time_to_string(const rtc_time_t *time, char *buffer, size_t buffer_size);
void rtc_time_to_tm(const rtc_time_t *time, struct tm *tm_time);
void rtc_tm_to_time(const struct tm *tm_time, rtc_time_t *time);
uint32_t rtc_get_unix_timestamp(const rtc_time_t *time);

#endif /* RTC_H */ 