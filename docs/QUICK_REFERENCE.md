# VDU_ESP32 - Quick Reference Guide

## 🚀 Quick Start

### **Build & Upload**
```bash
# Build and upload firmware
pio run -t upload

# Monitor serial output
pio device monitor

# Clean build
pio run -t clean
```

### **Hardware Connections**
```
ESP32 Pin    →    Component
GPIO 21      →    LCD SDA (I2C)
GPIO 22      →    LCD SCL (I2C)
GPIO 0       →    BOOT Button
GPIO 13      →    External Button (future)
5V           →    LCD VCC
3.3V         →    ESP32 VIN
GND          →    Common Ground
```

---

## 📋 Pin Definitions

### **Core Pins**
```c
// LCD I2C
#define VDU_LCD_I2C_SDA    21
#define VDU_LCD_I2C_SCL    22

// Buttons
#define VDU_BTN1           0     // BOOT button
#define VDU_BTN2           13    // External button

// Future CAN Bus
#define VDU_CAN_TX         5
#define VDU_CAN_RX         4
#define VDU_CAN_STBY       2
```

### **LCD Configuration**
```c
#define LCD_I2C_ADDR      0x27   // Try 0x3F if not working
#define LCD_COLS          16
#define LCD_ROWS          2
```

---

## 🔧 Key Functions

### **Dashboard API**
```c
// Initialize
dashboard_init();

// Update data
dashboard_update_data(&dashboard_data);

// Display page
dashboard_show_page(page, &dashboard_data);

// Navigation
dashboard_next_page();
dashboard_prev_page();
dashboard_get_current_page();

// Button handling
dashboard_check_buttons();
```

### **LCD API**
```c
// Initialize
lcd_i2c_init();

// Display control
lcd_i2c_clear();
lcd_i2c_set_cursor(col, row);
lcd_i2c_print("text");
```

### **GPIO API**
```c
// Initialize pins
vdu_pins_init();

// Read button
bool state = gpio_get_level(VDU_BTN1);
```

---

## 📊 Data Structures

### **Dashboard Data**
```c
typedef struct {
    unsigned int speed;        // km/h
    float odometer;           // km (XXXXXX.X format)
    unsigned int rpm;         // RPM
    unsigned int temperature; // °C
    unsigned int fuel_level;  // %
    unsigned int fuel_range;  // km
    float trip_distance;      // km
    unsigned int trip_time;   // seconds
} dashboard_data_t;
```

### **Dashboard Pages**
```c
typedef enum {
    DASHBOARD_PAGE_SPEED = 0,    // Speed & Odometer
    DASHBOARD_PAGE_ENGINE,       // RPM & Temperature
    DASHBOARD_PAGE_FUEL,         // Fuel Level & Range
    DASHBOARD_PAGE_TRIP,         // Trip Distance & Time
    DASHBOARD_PAGE_COMPACT,      // All data compact
    DASHBOARD_PAGE_COUNT
} dashboard_page_t;
```

---

## 🎮 User Interface

### **Display Layouts**
```
Page 0 (Speed):    Page 1 (Engine):    Page 2 (Fuel):
SPD:120 KMPH       RPM: 3000           FUEL: 85%
ODO:123456.7 KM    TEMP: 95°C          RANGE: 520 KM

Page 3 (Trip):     Page 4 (Compact):
TRIP: 00111.2 KM   SPD:120 FUEL:85%
TIME: 18:45        ODO:123456 95°C
```

### **Button Controls**
- **BOOT Button**: Navigate between pages
- **Page Cycle**: Speed → Engine → Fuel → Trip → Compact → Speed

---

## ⚙️ Configuration

### **Build Flags**
```ini
# platformio.ini
build_flags = 
    -Os                    # Size optimization
    -DCONFIG_ESP32_DEFAULT_CPU_FREQ_240=1
    -DCAN_BUS_ENABLED=1    # Future CAN support
```

### **System Settings**
```c
#define UPDATE_INTERVAL_MS    10     // 100Hz update rate
#define BUTTON_DEBOUNCE_MS    50     // Button debounce
#define BUTTON_CHECK_MS       10     // Button check interval
```

---

## 🐛 Common Issues

### **LCD Not Working**
```c
// Check I2C address
#define LCD_I2C_ADDR      0x27   // Try 0x3F

// Add debug
printf("LCD init status: %d\n", lcd_initialized);
```

### **Button Not Responding**
```c
// Check button state
printf("Button state: %d\n", gpio_get_level(VDU_BTN1));

// Verify GPIO config
gpio_config_t io_conf = {
    .pin_bit_mask = (1ULL << VDU_BTN1),
    .mode = GPIO_MODE_INPUT,
    .pull_up_en = 1,
};
```

### **Build Errors**
```bash
# Clean and rebuild
pio run -t clean
pio run -t upload

# Check dependencies
pio lib list
```

---

## 📁 File Structure

```
VDU_ESP32/
├── include/                 # Headers
│   ├── pins.h              # Pin definitions
│   ├── dashboard.h          # Dashboard interface
│   └── ...
├── lib/                     # Libraries
│   ├── I2CLcd/             # LCD driver
│   └── VDUDisplay/         # Display logic
├── src/                     # Source files
│   ├── main.c              # Main application
│   ├── dashboard.c         # Dashboard logic
│   ├── pins.c              # GPIO config
│   └── ...
├── docs/                    # Documentation
│   ├── PROGRAMMER_MANUAL.md
│   ├── SYSTEM_INTEGRATOR_MANUAL.md
│   └── QUICK_REFERENCE.md
└── platformio.ini          # Build config
```

---

## 🔄 Main Loop

### **Application Flow**
```c
void app_main(void) {
    // 1. Initialize system
    system_print_all_info();
    vdu_pins_init();
    lcd_i2c_init();
    serial_init();
    dashboard_init();
    
    // 2. Main loop
    while (1) {
        // Update simulated data
        speed = 100 + (speed - 100 + 1) % 21;
        update_odometer();
        
        // Update dashboard
        dashboard_data.speed = speed;
        dashboard_data.odometer = odo;
        dashboard_update_data(&dashboard_data);
        
        // Handle user input
        dashboard_check_buttons();
        
        // Update display
        dashboard_show_page(dashboard_get_current_page(), &dashboard_data);
        
        // Wait for next cycle
        vTaskDelay(pdMS_TO_TICKS(10));
    }
}
```

---

## 📊 Performance Metrics

### **System Specifications**
- **CPU**: ESP32 240MHz
- **RAM**: 320KB (11KB used - 3.4%)
- **Flash**: 4MB (226KB used - 21.5%)
- **Update Rate**: 100Hz (10ms cycle)
- **Power**: ~150mA @ 5V

### **Timing**
- **Button Response**: < 10ms
- **Display Update**: < 5ms
- **Main Loop**: 10ms
- **I2C Speed**: 50kHz

---

## 🚀 Adding Features

### **New Dashboard Page**
```c
// 1. Add to enum
typedef enum {
    // ... existing pages ...
    DASHBOARD_PAGE_NEW,
    DASHBOARD_PAGE_COUNT
} dashboard_page_t;

// 2. Add data field
typedef struct {
    // ... existing fields ...
    unsigned int new_field;
} dashboard_data_t;

// 3. Implement display
case DASHBOARD_PAGE_NEW:
    lcd_i2c_set_cursor(0, 0);
    snprintf(buf, sizeof(buf), "NEW: %3u", data->new_field);
    lcd_i2c_print(buf);
    break;
```

### **New Hardware**
```c
// 1. Define pin
#define VDU_NEW_DEVICE_PIN    14

// 2. Configure GPIO
gpio_config_t new_conf = {
    .pin_bit_mask = (1ULL << VDU_NEW_DEVICE_PIN),
    .mode = GPIO_MODE_OUTPUT,
};
gpio_config(&new_conf);
```

---

## 📞 Support

### **Documentation**
- **Programmer Manual**: `docs/PROGRAMMER_MANUAL.md`
- **System Integrator Manual**: `docs/SYSTEM_INTEGRATOR_MANUAL.md`
- **GitHub**: https://github.com/shantanuk47/VDU_ESP32

### **Contact**
- **Author**: Shantanu Kumar
- **GitHub**: @shantanuk47

---

## 🔄 Version History

### **v1.0.0 (Current)**
- ✅ Multi-page dashboard system
- ✅ BOOT button navigation
- ✅ 16x2 I2C LCD display
- ✅ Simulated vehicle data
- ✅ 100Hz update rate
- ✅ Comprehensive documentation

### **Planned Features**
- 🔄 CAN bus integration
- 🔄 External buttons
- 🔄 WiFi connectivity
- 🔄 Data logging
- 🔄 Real sensors

---

**Last Updated**: August 1, 2025  
**Version**: 1.0.0  
**Author**: Shantanu Kumar 