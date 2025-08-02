/***************************************************************
 * rtc.c
 * DS3231 RTC library implementation for VDU_ESP32
 * 
 * Project: VDU_ESP32 (Vehicle Display Unit)
 * Author: Shantanu Kumar
 * Date: 2025-01-15
 ***************************************************************/

#include "rtc.h"
#include "driver/i2c.h"
#include "esp_log.h"
#include "esp_err.h"
#include "pins.h"
#include <string.h>
#include <time.h>

static const char *TAG = "RTC";

/* I2C Configuration - Using pins from pins.h */
#define I2C_MASTER_SCL_IO           VDU_I2C_SCL      /* GPIO 22 for SCL */
#define I2C_MASTER_SDA_IO           VDU_I2C_SDA      /* GPIO 21 for SDA */
#define I2C_MASTER_NUM              I2C_NUM_0
#define I2C_MASTER_FREQ_HZ          50000   /* 50kHz - same as LCD */
#define I2C_MASTER_TX_BUF_DISABLE   0
#define I2C_MASTER_RX_BUF_DISABLE   0
#define I2C_MASTER_TIMEOUT_MS       1000

/* BCD to Decimal conversion */
static uint8_t bcd_to_decimal(uint8_t bcd) {
    return ((bcd >> 4) * 10) + (bcd & 0x0F);
}

/* Decimal to BCD conversion */
static uint8_t decimal_to_bcd(uint8_t decimal) {
    return ((decimal / 10) << 4) | (decimal % 10);
}

/* Write byte to RTC register */
static esp_err_t rtc_write_register(uint8_t reg, uint8_t data) {
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    if (cmd == NULL) {
        ESP_LOGE(TAG, "Failed to create I2C command link");
        return ESP_FAIL;
    }
    
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, (VDU_RTC_I2C_ADDR << 1) | I2C_MASTER_WRITE, true);
    i2c_master_write_byte(cmd, reg, true);
    i2c_master_write_byte(cmd, data, true);
    i2c_master_stop(cmd);
    
    esp_err_t ret = i2c_master_cmd_begin(I2C_MASTER_NUM, cmd, I2C_MASTER_TIMEOUT_MS / portTICK_PERIOD_MS);
    i2c_cmd_link_delete(cmd);
    
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "RTC write failed: reg=0x%02X, data=0x%02X, err=%s", reg, data, esp_err_to_name(ret));
    }
    
    /* Small delay after write */
    vTaskDelay(pdMS_TO_TICKS(10));
    
    return ret;
}

/* Read byte from RTC register */
static esp_err_t rtc_read_register(uint8_t reg, uint8_t *data) {
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, (VDU_RTC_I2C_ADDR << 1) | I2C_MASTER_WRITE, true);
    i2c_master_write_byte(cmd, reg, true);
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, (VDU_RTC_I2C_ADDR << 1) | I2C_MASTER_READ, true);
    i2c_master_read_byte(cmd, data, I2C_MASTER_NACK);
    i2c_master_stop(cmd);
    esp_err_t ret = i2c_master_cmd_begin(I2C_MASTER_NUM, cmd, I2C_MASTER_TIMEOUT_MS / portTICK_PERIOD_MS);
    i2c_cmd_link_delete(cmd);
    return ret;
}

/* Read multiple bytes from RTC registers */
static esp_err_t rtc_read_registers(uint8_t reg, uint8_t *data, size_t len) {
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, (VDU_RTC_I2C_ADDR << 1) | I2C_MASTER_WRITE, true);
    i2c_master_write_byte(cmd, reg, true);
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, (VDU_RTC_I2C_ADDR << 1) | I2C_MASTER_READ, true);
    if (len > 1) {
        i2c_master_read(cmd, data, len - 1, I2C_MASTER_ACK);
    }
    i2c_master_read_byte(cmd, data + len - 1, I2C_MASTER_NACK);
    i2c_master_stop(cmd);
    esp_err_t ret = i2c_master_cmd_begin(I2C_MASTER_NUM, cmd, I2C_MASTER_TIMEOUT_MS / portTICK_PERIOD_MS);
    i2c_cmd_link_delete(cmd);
    return ret;
}

/* Initialize RTC */
esp_err_t ds3231_init(void) {
    /* Skip I2C configuration - LCD already configured it */
    ESP_LOGI(TAG, "Using existing I2C configuration (from LCD)");
    
    /* Test RTC communication */
    uint8_t test_data;
    esp_err_t ret = rtc_read_register(DS3231_REG_SECONDS, &test_data);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "RTC communication test failed: %s", esp_err_to_name(ret));
        return ret;
    }
    
    ESP_LOGI(TAG, "RTC initialized successfully");
    return ESP_OK;
}

/* Set RTC time */
esp_err_t ds3231_set_time(const rtc_time_t *time) {
    esp_err_t ret;
    
    /* Set seconds */
    ret = rtc_write_register(DS3231_REG_SECONDS, decimal_to_bcd(time->seconds));
    if (ret != ESP_OK) return ret;
    
    /* Set minutes */
    ret = rtc_write_register(DS3231_REG_MINUTES, decimal_to_bcd(time->minutes));
    if (ret != ESP_OK) return ret;
    
    /* Set hours */
    ret = rtc_write_register(DS3231_REG_HOURS, decimal_to_bcd(time->hours));
    if (ret != ESP_OK) return ret;
    
    /* Set day of week */
    ret = rtc_write_register(DS3231_REG_DAY, decimal_to_bcd(time->day_of_week));
    if (ret != ESP_OK) return ret;
    
    /* Set date */
    ret = rtc_write_register(DS3231_REG_DATE, decimal_to_bcd(time->date));
    if (ret != ESP_OK) return ret;
    
    /* Set month */
    ret = rtc_write_register(DS3231_REG_MONTH, decimal_to_bcd(time->month));
    if (ret != ESP_OK) return ret;
    
    /* Set year (2-digit) */
    ret = rtc_write_register(DS3231_REG_YEAR, decimal_to_bcd(time->year - 2000));
    if (ret != ESP_OK) return ret;
    
    return ESP_OK;
}

/* Get RTC time */
esp_err_t ds3231_get_time(rtc_time_t *time) {
    uint8_t data[7];
    esp_err_t ret;
    
    ret = rtc_read_registers(DS3231_REG_SECONDS, data, 7);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to read RTC time: %s", esp_err_to_name(ret));
        return ret;
    }
    
    time->seconds = bcd_to_decimal(data[0] & 0x7F);
    time->minutes = bcd_to_decimal(data[1] & 0x7F);
    time->hours = bcd_to_decimal(data[2] & 0x3F);
    time->day_of_week = bcd_to_decimal(data[3] & 0x07);
    time->date = bcd_to_decimal(data[4] & 0x3F);
    time->month = bcd_to_decimal(data[5] & 0x1F);
    time->year = 2000 + bcd_to_decimal(data[6]);
    
    return ESP_OK;
}

/* Get RTC temperature */
esp_err_t ds3231_get_temperature(rtc_temperature_t *temp) {
    uint8_t data[2];
    esp_err_t ret;
    
    ret = rtc_read_registers(DS3231_REG_TEMP_MSB, data, 2);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to read RTC temperature: %s", esp_err_to_name(ret));
        return ret;
    }
    
    temp->temperature = (int16_t)((data[0] << 8) | data[1]);
    temp->temperature_celsius = (float)temp->temperature / 256.0;
    
    return ESP_OK;
}

/* Check if oscillator has stopped */
bool ds3231_is_oscillator_stopped(void) {
    uint8_t status;
    esp_err_t ret = rtc_read_register(DS3231_REG_STATUS, &status);
    if (ret != ESP_OK) {
        return true; /* Assume stopped if we can't read */
    }
    return (status & DS3231_STATUS_OSF) != 0;
}

/* Clear alarm flags */
esp_err_t ds3231_clear_alarm_flags(void) {
    uint8_t status;
    esp_err_t ret = rtc_read_register(DS3231_REG_STATUS, &status);
    if (ret != ESP_OK) {
        return ret;
    }
    
    status &= ~(DS3231_STATUS_A1F | DS3231_STATUS_A2F);
    return rtc_write_register(DS3231_REG_STATUS, status);
}

/* Get status register */
esp_err_t ds3231_get_status(uint8_t *status) {
    return rtc_read_register(DS3231_REG_STATUS, status);
}

/* Set control register */
esp_err_t ds3231_set_control(uint8_t control) {
    return rtc_write_register(DS3231_REG_CONTROL, control);
}

/* Get control register */
esp_err_t ds3231_get_control(uint8_t *control) {
    return rtc_read_register(DS3231_REG_CONTROL, control);
}

/* Force temperature conversion */
esp_err_t ds3231_force_temperature_conversion(void) {
    uint8_t control;
    esp_err_t ret = rtc_read_register(DS3231_REG_CONTROL, &control);
    if (ret != ESP_OK) {
        return ret;
    }
    
    control |= DS3231_CONTROL_CONV;
    return rtc_write_register(DS3231_REG_CONTROL, control);
}

/* Convert time to string (HH:MM:SS format) */
void rtc_time_to_string(const rtc_time_t *time, char *buffer, size_t buffer_size) {
    snprintf(buffer, buffer_size, "%02u:%02u:%02u", 
             time->hours, time->minutes, time->seconds);
}

/* Convert time to struct tm */
void rtc_time_to_tm(const rtc_time_t *time, struct tm *tm_time) {
    memset(tm_time, 0, sizeof(struct tm));
    tm_time->tm_sec = time->seconds;
    tm_time->tm_min = time->minutes;
    tm_time->tm_hour = time->hours;
    tm_time->tm_mday = time->date;
    tm_time->tm_mon = time->month - 1; /* struct tm uses 0-based months */
    tm_time->tm_year = time->year - 1900; /* struct tm uses years since 1900 */
    tm_time->tm_wday = time->day_of_week - 1; /* struct tm uses 0-based days */
}

/* Convert struct tm to time */
void rtc_tm_to_time(const struct tm *tm_time, rtc_time_t *time) {
    time->seconds = tm_time->tm_sec;
    time->minutes = tm_time->tm_min;
    time->hours = tm_time->tm_hour;
    time->date = tm_time->tm_mday;
    time->month = tm_time->tm_mon + 1; /* Convert back to 1-based months */
    time->year = tm_time->tm_year + 1900; /* Convert back to full year */
    time->day_of_week = tm_time->tm_wday + 1; /* Convert back to 1-based days */
}

/* Get Unix timestamp */
uint32_t rtc_get_unix_timestamp(const rtc_time_t *time) {
    struct tm tm_time;
    rtc_time_to_tm(time, &tm_time);
    return (uint32_t)mktime(&tm_time);
} 