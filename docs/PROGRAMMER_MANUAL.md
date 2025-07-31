# VDU_ESP32 - Programmer's Manual

## 📋 Table of Contents
1. [System Architecture](#system-architecture)
2. [Code Structure](#code-structure)
3. [Data Flow](#data-flow)
4. [Core Components](#core-components)
5. [Adding New Features](#adding-new-features)
6. [Debugging Guide](#debugging-guide)
7. [Best Practices](#best-practices)
8. [API Reference](#api-reference)

---

## 🏗️ System Architecture

### **High-Level Overview**
```
┌─────────────────┐    ┌─────────────────┐    ┌─────────────────┐
│   Main Loop     │───▶│  Dashboard      │───▶│   LCD Display   │
│   (main.c)      │    │  System         │    │   (I2C)         │
└─────────────────┘    └─────────────────┘    └─────────────────┘
         │                       │                       │
         ▼                       ▼                       ▼
┌─────────────────┐    ┌─────────────────┐    ┌─────────────────┐
│   Button Input  │    │   Data Update   │    │   Serial Comm   │
│   (GPIO)        │    │   (Simulation)  │    │   (UART)        │
└─────────────────┘    └─────────────────┘    └─────────────────┘
```

### **Task Scheduling**
- **Main Task**: 10ms loop (100Hz)
- **Button Check**: Every 10ms with debouncing
- **LCD Update**: On-demand (page change or data update)
- **Serial Handler**: Background task

---

## 📁 Code Structure

### **Directory Organization**
```
VDU_ESP32/
├── include/                 # Public headers
│   ├── pins.h              # Pin definitions
│   ├── dashboard.h          # Dashboard interface
│   ├── serial.h             # Serial communication
│   └── system_util.h        # System utilities
├── lib/                     # Custom libraries
│   ├── I2CLcd/             # LCD driver
│   │   ├── lcd_i2c.c       # Implementation
│   │   └── lcd_i2c.h       # Interface
│   └── VDUDisplay/         # Display logic
│       ├── vdu_display.c   # Implementation
│       └── vdu_display.h   # Interface
├── src/                     # Main source files
│   ├── main.c              # Application entry
│   ├── dashboard.c         # Dashboard logic
│   ├── pins.c              # GPIO configuration
│   ├── serial.c            # Serial communication
│   └── system_util.c       # System utilities
└── docs/                    # Documentation
```

### **File Responsibilities**

| File | Purpose | Dependencies |
|------|---------|--------------|
| `main.c` | Application entry point, main loop | All modules |
| `dashboard.c` | Multi-page display logic | LCD, GPIO |
| `pins.c` | GPIO configuration | ESP-IDF GPIO |
| `serial.c` | UART communication | ESP-IDF UART |
| `system_util.c` | System information | ESP-IDF system |
| `lcd_i2c.c` | I2C LCD driver | ESP-IDF I2C |
| `vdu_display.c` | Display formatting | LCD driver |

---

## 🔄 Data Flow

### **1. Application Initialization**
```c
app_main() {
    system_print_all_info();     // System info
    vdu_pins_init();             // GPIO setup
    lcd_i2c_init();              // LCD initialization
    serial_init();               // Serial setup
    dashboard_init();            // Dashboard setup
    
    // Main loop starts
}
```

### **2. Main Loop Execution**
```c
while (1) {
    // 1. Update simulated data
    speed = 100 + (speed - 100 + 1) % 21;
    update_odometer();
    
    // 2. Update dashboard data structure
    dashboard_data.speed = speed;
    dashboard_data.odometer = odo;
    dashboard_update_data(&dashboard_data);
    
    // 3. Check user input
    dashboard_check_buttons();
    
    // 4. Update display
    dashboard_show_page(dashboard_get_current_page(), &dashboard_data);
    
    // 5. Wait for next cycle
    vTaskDelay(pdMS_TO_TICKS(10));
}
```

### **3. Button Event Flow**
```
Button Press → GPIO Read → Debounce → Page Change → Display Update
```

### **4. Data Update Flow**
```
Simulation → Data Structure → Dashboard Update → LCD Display
```

---

## 🔧 Core Components

### **1. Dashboard System (`dashboard.c`)**

#### **Key Data Structures**
```c
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

typedef enum {
    DASHBOARD_PAGE_SPEED = 0,
    DASHBOARD_PAGE_ENGINE,
    DASHBOARD_PAGE_FUEL,
    DASHBOARD_PAGE_TRIP,
    DASHBOARD_PAGE_COMPACT,
    DASHBOARD_PAGE_COUNT
} dashboard_page_t;
```

#### **State Management**
```c
static dashboard_page_t current_page = DASHBOARD_PAGE_SPEED;
static unsigned long last_button_check = 0;
static bool last_btn1_state = true;
```

#### **Key Functions**
- `dashboard_init()`: Initialize dashboard state
- `dashboard_update_data()`: Update simulated values
- `dashboard_show_page()`: Display specific page
- `dashboard_check_buttons()`: Handle button input
- `dashboard_next_page()`: Navigate to next page

### **2. LCD Driver (`lcd_i2c.c`)**

#### **Configuration**
```c
#define LCD_I2C_ADDR      0x27   // I2C address
#define LCD_COLS          16     // Display width
#define LCD_ROWS          2      // Display height
#define I2C_PORT          I2C_NUM_0
```

#### **Key Functions**
- `lcd_i2c_init()`: Initialize I2C and LCD
- `lcd_i2c_clear()`: Clear display
- `lcd_i2c_set_cursor()`: Set cursor position
- `lcd_i2c_print()`: Print string
- `lcd_send_byte()`: Send byte to LCD

### **3. GPIO Management (`pins.c`)**

#### **Pin Definitions**
```c
#define VDU_LCD_I2C_SDA    21    // I2C SDA
#define VDU_LCD_I2C_SCL    22    // I2C SCL
#define VDU_BTN1           0     // BOOT button
#define VDU_BTN2           13    // External button
```

#### **Configuration**
```c
void vdu_pins_init(void) {
    gpio_config_t io_conf = {
        .intr_type = GPIO_INTR_DISABLE,
        .mode = GPIO_MODE_INPUT,
        .pin_bit_mask = (1ULL << VDU_BTN1) | (1ULL << VDU_BTN2),
        .pull_up_en = 1,
    };
    gpio_config(&io_conf);
}
```

---

## 🚀 Adding New Features

### **1. Adding a New Dashboard Page**

#### **Step 1: Update Header (`include/dashboard.h`)**
```c
typedef enum {
    DASHBOARD_PAGE_SPEED = 0,
    DASHBOARD_PAGE_ENGINE,
    DASHBOARD_PAGE_FUEL,
    DASHBOARD_PAGE_TRIP,
    DASHBOARD_PAGE_COMPACT,
    DASHBOARD_PAGE_NEW,        // Add new page
    DASHBOARD_PAGE_COUNT
} dashboard_page_t;

typedef struct {
    // ... existing fields ...
    unsigned int new_field;    // Add new data field
} dashboard_data_t;
```

#### **Step 2: Update Implementation (`src/dashboard.c`)**
```c
void dashboard_update_data(dashboard_data_t *data) {
    // ... existing code ...
    data->new_field = calculate_new_value();  // Add calculation
}

void dashboard_show_page(dashboard_page_t page, dashboard_data_t *data) {
    switch (page) {
        // ... existing cases ...
        case DASHBOARD_PAGE_NEW:
            lcd_i2c_set_cursor(0, 0);
            snprintf(buf, sizeof(buf), "NEW: %3u", data->new_field);
            lcd_i2c_print(buf);
            break;
    }
}
```

### **2. Adding New Hardware**

#### **Step 1: Define Pins (`include/pins.h`)**
```c
#define VDU_NEW_DEVICE_PIN    14    // New device pin
```

#### **Step 2: Configure GPIO (`src/pins.c`)**
```c
void vdu_pins_init(void) {
    // ... existing code ...
    
    // Configure new device pin
    gpio_config_t new_conf = {
        .pin_bit_mask = (1ULL << VDU_NEW_DEVICE_PIN),
        .mode = GPIO_MODE_OUTPUT,  // or INPUT as needed
    };
    gpio_config(&new_conf);
}
```

#### **Step 3: Create Driver (`lib/NewDevice/`)**
```c
// new_device.h
void new_device_init(void);
void new_device_read(void);
void new_device_write(uint8_t data);

// new_device.c
#include "new_device.h"
#include "pins.h"

void new_device_init(void) {
    // Initialize new device
}

void new_device_read(void) {
    // Read from device
}
```

### **3. Adding New Data Sources**

#### **Step 1: Update Data Structure**
```c
typedef struct {
    // ... existing fields ...
    float new_sensor_value;
    unsigned int new_status;
} dashboard_data_t;
```

#### **Step 2: Add Data Collection**
```c
void collect_real_data(dashboard_data_t *data) {
    // Read from real sensors
    data->new_sensor_value = read_sensor();
    data->new_status = get_device_status();
}
```

#### **Step 3: Integrate into Main Loop**
```c
while (1) {
    // ... existing code ...
    
    // Add real data collection
    collect_real_data(&dashboard_data);
    
    // ... rest of loop ...
}
```

---

## 🐛 Debugging Guide

### **1. Common Issues**

#### **LCD Not Working**
```c
// Check I2C address
#define LCD_I2C_ADDR      0x27   // Try 0x3F if not working

// Add debug prints
printf("LCD init status: %d\n", lcd_initialized);
printf("I2C address: 0x%02X\n", LCD_I2C_ADDR);
```

#### **Button Not Responding**
```c
// Check button state
printf("Button state: %d\n", gpio_get_level(VDU_BTN1));

// Add button debug
if (!current_btn1_state && last_btn1_state) {
    printf("Button pressed!\n");
}
```

#### **Memory Issues**
```c
// Check memory usage
printf("Free heap: %d bytes\n", esp_get_free_heap_size());
printf("Minimum free heap: %d bytes\n", esp_get_minimum_free_heap_size());
```

### **2. Debug Tools**

#### **Serial Monitor**
```bash
pio device monitor
```

#### **I2C Scanner**
```c
void i2c_scan_devices(void) {
    for (int i = 0; i < 128; i++) {
        // Scan for devices
        if (device_found) {
            printf("Device at 0x%02X\n", i);
        }
    }
}
```

#### **Performance Monitoring**
```c
unsigned long start_time = xTaskGetTickCount();
// ... code to measure ...
unsigned long end_time = xTaskGetTickCount();
printf("Execution time: %lu ms\n", (end_time - start_time) * portTICK_PERIOD_MS);
```

---

## 📋 Best Practices

### **1. Code Organization**
- Keep functions small and focused
- Use meaningful variable names
- Add comments for complex logic
- Follow consistent formatting

### **2. Error Handling**
```c
esp_err_t result = some_function();
if (result != ESP_OK) {
    printf("Error: %s\n", esp_err_to_name(result));
    // Handle error appropriately
}
```

### **3. Memory Management**
- Use static allocation when possible
- Check heap usage regularly
- Avoid memory leaks in loops
- Use appropriate data types

### **4. Performance**
- Minimize blocking operations
- Use efficient data structures
- Optimize critical paths
- Monitor execution times

### **5. Documentation**
- Document all public APIs
- Include usage examples
- Update documentation with code changes
- Use clear and concise language

---

## 📚 API Reference

### **Dashboard API**
```c
// Initialize dashboard
void dashboard_init(void);

// Update data structure
void dashboard_update_data(dashboard_data_t *data);

// Display specific page
void dashboard_show_page(dashboard_page_t page, dashboard_data_t *data);

// Navigation
void dashboard_next_page(void);
void dashboard_prev_page(void);
dashboard_page_t dashboard_get_current_page(void);

// Button handling
void dashboard_check_buttons(void);
```

### **LCD API**
```c
// Initialization
void lcd_i2c_init(void);
void lcd_i2c_deinit(void);

// Display control
void lcd_i2c_clear(void);
void lcd_i2c_set_cursor(uint8_t col, uint8_t row);
void lcd_i2c_print(const char *str);
```

### **GPIO API**
```c
// Pin configuration
void vdu_pins_init(void);

// Button reading
bool gpio_get_level(gpio_num_t gpio_num);
```

### **System API**
```c
// System information
void system_print_all_info(void);

// Serial communication
void serial_init(void);
```

---

## 🔄 Version Control

### **Branch Strategy**
- `main`: Stable production code
- `develop`: Integration branch
- `feature/*`: New features
- `bugfix/*`: Bug fixes
- `hotfix/*`: Critical fixes

### **Commit Guidelines**
```
feat: add new dashboard page
fix: resolve LCD initialization issue
docs: update API documentation
refactor: improve button handling
test: add unit tests for dashboard
```

### **Code Review Checklist**
- [ ] Code compiles without warnings
- [ ] All tests pass
- [ ] Documentation updated
- [ ] Error handling implemented
- [ ] Performance impact assessed
- [ ] Security considerations reviewed

---

## 📞 Support

### **Getting Help**
1. Check this documentation
2. Review existing issues
3. Search code comments
4. Contact the development team

### **Contributing**
1. Fork the repository
2. Create feature branch
3. Make changes
4. Test thoroughly
5. Submit pull request

---

**Last Updated**: August 1, 2025  
**Version**: 1.0.0  
**Author**: Shantanu Kumar 