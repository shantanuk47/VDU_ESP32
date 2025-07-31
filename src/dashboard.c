#include "dashboard.h"
#include "vdu_display.h"
#include "lcd_i2c.h"
#include "pins.h"
#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include <stdio.h>

/* Current dashboard state */
static dashboard_page_t current_page = DASHBOARD_PAGE_SPEED;
static unsigned long last_button_check = 0;
static bool last_btn1_state = true; /* Button is HIGH when not pressed (pull-up) */

/* Button debouncing */
#define BUTTON_DEBOUNCE_MS 50
#define BUTTON_CHECK_INTERVAL_MS 10

/* Initialize dashboard */
void dashboard_init(void)
{
    current_page = DASHBOARD_PAGE_SPEED;
    last_button_check = 0;
    last_btn1_state = true;
}

/* Update dashboard data with current values */
void dashboard_update_data(dashboard_data_t *data)
{
    /* Speed and odometer are updated in main.c */
    /* For now, simulate other values */
    data->rpm = data->speed * 25; /* Simulate RPM based on speed */
    data->temperature = 80 + (data->speed % 20); /* Simulate temperature */
    data->fuel_level = 65 + (data->speed % 30); /* Simulate fuel level */
    data->fuel_range = 400 + (data->speed * 2); /* Simulate fuel range */
    data->trip_distance = data->odometer - 12345.0; /* Simulate trip distance */
    data->trip_time = (data->speed * 10) % 3600; /* Simulate trip time in seconds */
}

/* Show specific dashboard page */
void dashboard_show_page(dashboard_page_t page, dashboard_data_t *data)
{
    char buf[32]; /* Increased buffer size to avoid truncation warnings */
    
    switch (page) {
        case DASHBOARD_PAGE_SPEED:
            /* Line 1: Speed */
            lcd_i2c_set_cursor(0, 0);
            snprintf(buf, sizeof(buf), "SPD:%3u KMPH    ", data->speed);
            lcd_i2c_print(buf);
            
            /* Line 2: Odometer */
            lcd_i2c_set_cursor(0, 1);
            snprintf(buf, sizeof(buf), "ODO:%08.1f KM  ", data->odometer);
            lcd_i2c_print(buf);
            break;
            
        case DASHBOARD_PAGE_ENGINE:
            /* Line 1: RPM */
            lcd_i2c_set_cursor(0, 0);
            snprintf(buf, sizeof(buf), "RPM: %4u      ", data->rpm);
            lcd_i2c_print(buf);
            
            /* Line 2: Temperature */
            lcd_i2c_set_cursor(0, 1);
            snprintf(buf, sizeof(buf), "TEMP: %2u°C     ", data->temperature);
            lcd_i2c_print(buf);
            break;
            
        case DASHBOARD_PAGE_FUEL:
            /* Line 1: Fuel Level */
            lcd_i2c_set_cursor(0, 0);
            snprintf(buf, sizeof(buf), "FUEL: %2u%%      ", data->fuel_level);
            lcd_i2c_print(buf);
            
            /* Line 2: Fuel Range */
            lcd_i2c_set_cursor(0, 1);
            snprintf(buf, sizeof(buf), "RANGE: %3u KM  ", data->fuel_range);
            lcd_i2c_print(buf);
            break;
            
        case DASHBOARD_PAGE_TRIP:
            /* Line 1: Trip Distance */
            lcd_i2c_set_cursor(0, 0);
            snprintf(buf, sizeof(buf), "TRIP: %05.1f KM", data->trip_distance);
            lcd_i2c_print(buf);
            
            /* Line 2: Trip Time */
            lcd_i2c_set_cursor(0, 1);
            unsigned int minutes = data->trip_time / 60;
            unsigned int seconds = data->trip_time % 60;
            snprintf(buf, sizeof(buf), "TIME: %02u:%02u", minutes, seconds);
            lcd_i2c_print(buf);
            break;
            
        case DASHBOARD_PAGE_COMPACT:
            /* Line 1: Speed + Fuel */
            lcd_i2c_set_cursor(0, 0);
            snprintf(buf, sizeof(buf), "SPD:%3u FUEL:%2u%%", data->speed, data->fuel_level);
            lcd_i2c_print(buf);
            
            /* Line 2: Odo + Temp */
            lcd_i2c_set_cursor(0, 1);
            snprintf(buf, sizeof(buf), "ODO:%06.0f %2u°C", data->odometer, data->temperature);
            lcd_i2c_print(buf);
            break;
            
        default:
            /* Fallback to speed page */
            dashboard_show_page(DASHBOARD_PAGE_SPEED, data);
            break;
    }
}

/* Navigate to next page */
void dashboard_next_page(void)
{
    current_page = (current_page + 1) % DASHBOARD_PAGE_COUNT;
}

/* Navigate to previous page */
void dashboard_prev_page(void)
{
    if (current_page == 0) {
        current_page = DASHBOARD_PAGE_COUNT - 1;
    } else {
        current_page--;
    }
}

/* Get current page */
dashboard_page_t dashboard_get_current_page(void)
{
    return current_page;
}

/* Check button states and handle navigation */
void dashboard_check_buttons(void)
{
    unsigned long current_time = xTaskGetTickCount() * portTICK_PERIOD_MS;
    
    /* Check buttons every 10ms */
    if (current_time - last_button_check >= BUTTON_CHECK_INTERVAL_MS) {
        /* Read BOOT button (GPIO 0) */
        bool current_btn1_state = gpio_get_level(VDU_BTN1);
        
        /* Button pressed (LOW) - simplified logic */
        if (!current_btn1_state && last_btn1_state) {
            /* Wait a bit for debounce */
            vTaskDelay(pdMS_TO_TICKS(10));
            current_btn1_state = gpio_get_level(VDU_BTN1);
            
            /* Still pressed after debounce */
            if (!current_btn1_state) {
                /* BOOT button pressed - go to next page */
                dashboard_next_page();
                printf("Page: %d\n", current_page);
            }
        }
        
        last_btn1_state = current_btn1_state;
        last_button_check = current_time;
    }
} 