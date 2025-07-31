#ifndef DASHBOARD_H
#define DASHBOARD_H

#include <stdint.h>

/* Dashboard page definitions */
typedef enum {
    DASHBOARD_PAGE_SPEED = 0,
    DASHBOARD_PAGE_ENGINE,
    DASHBOARD_PAGE_FUEL,
    DASHBOARD_PAGE_TRIP,
    DASHBOARD_PAGE_COMPACT,
    DASHBOARD_PAGE_COUNT
} dashboard_page_t;

/* Dashboard data structure */
typedef struct {
    unsigned int speed;
    float odometer;
    unsigned int rpm;
    unsigned int temperature;
    unsigned int fuel_level;
    unsigned int fuel_range;
    float trip_distance;
    unsigned int trip_time;
} dashboard_data_t;

/* Dashboard functions */
void dashboard_init(void);
void dashboard_update_data(dashboard_data_t *data);
void dashboard_show_page(dashboard_page_t page, dashboard_data_t *data);
void dashboard_next_page(void);
void dashboard_prev_page(void);
dashboard_page_t dashboard_get_current_page(void);
void dashboard_check_buttons(void);

#endif /* DASHBOARD_H */ 