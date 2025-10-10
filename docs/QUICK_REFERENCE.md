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
GPIO 21      →    LCD SDA (I2C) + RTC SDA
GPIO 22      →    LCD SCL (I2C) + RTC SCL
GPIO 0       →    BOOT Button
GPIO 4       →    CAN RX (MCP2551)
GPIO 5       →    CAN TX (MCP2551)
5V           →    LCD VCC, MCP2551 VCC, RTC VCC
3.3V         →    ESP32 VIN
GND          →    Common Ground
```

### **Desktop Application**
```bash
# Navigate to desktop app
cd VDU_Desktop_App

# Install dependencies
pip install -r requirements.txt

# Run GUI
python VDU_Manager.py

# Build executable
./build_exe.bat
```

---

## 📋 Pin Definitions

### **Core Pins**
```c
// LCD I2C (shared with RTC)
#define VDU_LCD_I2C_SDA    21
#define VDU_LCD_I2C_SCL    22

// Buttons
#define VDU_BTN1           0     // BOOT button
#define VDU_BTN2           13    // External button

// CAN Bus - MCP2551 Transceiver
#define VDU_CAN_TX         5   /* MCP2551 CTX pin */
#define VDU_CAN_RX         4   /* MCP2551 CRX pin */
```

### **LCD Configuration**
```c
#define LCD_I2C_ADDR      0x27   // Try 0x3F if not working
#define LCD_COLS          16
#define LCD_ROWS          2
```

### **RTC Configuration**
```c
#define VDU_RTC_I2C_ADDR  0x68   // DS3231 I2C address
#define I2C_MASTER_FREQ_HZ 50000 // 50kHz I2C speed
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

### **RTC API**
```c
// Initialize
ds3231_init();

// Get time
rtc_time_t time;
ds3231_get_time(&time);

// Set time
rtc_time_t new_time = {
    .seconds = 30,
    .minutes = 45,
    .hours = 14,
    .date = 2,
    .month = 8,
    .year = 2025
};
ds3231_set_time(&new_time);
```

### **CAN Bus API**
```c
// Initialize
can_init_default();  // 500kbps default

// Send message
can_message_t msg = {
    .id = 0x301,
    .length = 8,
    .is_extended = false,
    .is_rtr = false
};
can_send(&msg);
```

### **Serial Commands**
```bash
# System information
INFO

# Set RTC time
SET_TIME 2025 08 02 14 30 25

# Test command
TEST
```

---

## 🎮 User Interface

### **Dashboard Pages**
```
Page 0 (Speed):    Page 1 (Engine):    Page 2 (Fuel):
SPD:120 KMPH       RPM: 3000           FUEL: 85%
ODO:123456.7 KM    TEMP: 95°C          RANGE: 520 KM

Page 3 (Trip):     Page 4 (Compact):
TRIP: 00111.2 KM   SPD:120 FUEL:85%
TIME: 18:45        ODO:123456 95°C

Time Display (all pages):
D: 02 AUG 2025 T: 14:30:25 PM
```

### **Button Controls**
- **BOOT Button**: Navigate between pages
- **Page Cycle**: Speed → Engine → Fuel → Trip → Compact → Speed

---

## ⚙️ Configuration

### **Multi-rate System**
```c
// CAN: 10Hz (100ms) - Real-time vehicle data
const unsigned long CAN_SEND_INTERVAL_MS = 100;

// System: 100Hz (10ms) - Button handling and data processing
vTaskDelay(pdMS_TO_TICKS(10));

// Display: 2Hz (500ms) - LCD refresh rate
const unsigned long DISPLAY_UPDATE_INTERVAL_MS = 500;
```

### **CAN Message Format**
```c
// Vehicle data message (ID: 0x301)
typedef struct {
    uint8_t speed;        // Vehicle speed (km/h)
    uint8_t rpm_high;     // RPM high byte
    uint8_t rpm_low;      // RPM low byte
    uint8_t temperature;  // Engine temperature (°C)
    uint8_t fuel_level;   // Fuel level (%)
    uint8_t status;       // Status bits
    uint8_t checksum;     // Message checksum
} vehicle_data_t;
```

---

## 🐛 Troubleshooting

### **LCD Not Working**
```c
// Check I2C address
#define LCD_I2C_ADDR      0x27   // Try 0x3F if not working

// Verify connections
// SDA: GPIO 21, SCL: GPIO 22, VCC: 5V, GND: GND
```

### **RTC Not Working**
```c
// Check I2C address
#define VDU_RTC_I2C_ADDR  0x68   // DS3231 address

// Set time via serial
SET_TIME 2025 08 02 14 30 25

// Use desktop app for easy time setting
```

### **CAN Bus Issues**
```c
// Check connections
#define VDU_CAN_TX         5   // MCP2551 CTX
#define VDU_CAN_RX         4   // MCP2551 CRX

// Verify termination resistors (120Ω)
// Check CAN bus speed (500kbps)
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

### **Desktop App Issues**
```bash
# Check serial port availability
# Ensure ESP32 not in use by other apps
# Verify Python dependencies
pip install -r requirements.txt

# Build standalone executable
./build_exe.bat
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
│   ├── date_time.h          # Date/time handling
│   └── ...
├── lib/                     # Libraries
│   ├── I2CLcd/             # LCD driver
│   ├── VDUDisplay/         # Display logic
│   ├── CANBus/             # CAN bus communication
│   └── DS3231/             # RTC module
├── src/                     # Source files
│   ├── main.c              # Main application
│   ├── dashboard.c         # Dashboard logic
│   ├── date_time.c         # Date/time handling
│   ├── serial.c            # Serial commands
│   ├── pins.c              # GPIO config
│   └── ...
├── VDU_Desktop_App/        # Desktop GUI Application
│   ├── VDU_Manager.py      # Main GUI application
│   ├── build_exe.bat       # Build script
│   ├── requirements.txt    # Python dependencies
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
    can_init_default();  // Initialize CAN bus
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
        
        // Send CAN message every 100ms
        if (current_time - last_can_send >= 100) {
            // Prepare and send CAN message (ID 0x301)
            can_send(&can_msg);
            last_can_send = current_time;
        }
        
        // Handle user input
        dashboard_check_buttons();
        
        // Update display every 500ms
        if (current_time - last_display_update >= 500) {
            dashboard_show_page(dashboard_get_current_page(), &dashboard_data);
            last_display_update = current_time;
        }
        
        // Wait for next cycle
        vTaskDelay(pdMS_TO_TICKS(10));  // 10ms cycle (100Hz)
    }
}
```

---

## 📊 Performance Metrics

### **System Specifications**
- **CPU**: ESP32 240MHz
- **RAM**: 320KB (11KB used - 3.4%)
- **Flash**: 4MB (264KB used - 25.2%)
- **Multi-rate System**:
  - CAN: 10Hz (100ms) - Real-time vehicle data
  - System: 100Hz (10ms) - Button handling and data processing
  - Display: 2Hz (500ms) - LCD refresh rate
- **Power**: ~150mA @ 5V

### **Timing**
- **Button Response**: < 10ms
- **Display Update**: < 5ms
- **Main Loop**: 10ms (100Hz)
- **CAN Speed**: 500kbps
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

### **New Serial Command**
```c
// 1. Add command handler
static void cmd_new_command(void) {
    printf("New command executed!\n");
}

// 2. Add to command table
static const serial_command_t commands[] = {
    {"INFO", cmd_info},
    {"TEST", cmd_test},
    {"NEW_CMD", cmd_new_command},
    // ...
};
```

---

## 📞 Support

### **Documentation**
- **Programmer Manual**: `docs/PROGRAMMER_MANUAL.md`
- **System Integrator Manual**: `docs/SYSTEM_INTEGRATOR_MANUAL.md`
- **Desktop App Guide**: `VDU_Desktop_App/README.md`
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
- ✅ CAN bus integration (MCP2551)
- ✅ Real-time CAN transmission (ID 0x301)
- ✅ 500kbps CAN communication
- ✅ DS3231 RTC integration
- ✅ Date/time display on all pages
- ✅ Serial command interface
- ✅ Desktop management application
- ✅ Multi-rate system (CAN: 10Hz, System: 100Hz, Display: 2Hz)
- ✅ Comprehensive documentation

### **Planned Features**
- 🔄 CAN message reception and parsing
- 🔄 External buttons
- 🔄 WiFi connectivity
- 🔄 Data logging
- 🔄 Real sensors

---

**Last Updated**: August 2, 2025  
**Version**: 1.0.0  
**Author**: Shantanu Kumar 