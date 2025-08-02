# VDU_ESP32 - System Integrator's Manual

## 📋 Table of Contents
1. [System Overview](#system-overview)
2. [Hardware Integration](#hardware-integration)
3. [Software Integration](#software-integration)
4. [CAN Bus Integration](#can-bus-integration)
5. [Power Management](#power-management)
6. [Environmental Considerations](#environmental-considerations)
7. [Testing & Validation](#testing--validation)
8. [Deployment Guide](#deployment-guide)
9. [Maintenance & Support](#maintenance--support)

---

## 🏗️ System Overview

### **VDU System Architecture**
```
┌─────────────────────────────────────────────────────────────┐
│                    Vehicle System                           │
├─────────────────────────────────────────────────────────────┤
│  ┌─────────────┐  ┌─────────────┐  ┌─────────────┐        │
│  │   Engine    │  │   Sensors   │  │   CAN Bus   │        │
│  │   Control   │  │   (Temp,    │  │   Network   │        │
│  │   Unit      │  │   Pressure) │  │             │        │
│  └─────────────┘  └─────────────┘  └─────────────┘        │
│         │                │                │                │
│         └────────────────┼────────────────┘                │
│                          │                                 │
│  ┌─────────────────────────────────────────────────────────┐ │
│  │              VDU_ESP32 System                          │ │
│  │  ┌─────────────┐  ┌─────────────┐  ┌─────────────┐    │ │
│  │  │   ESP32     │  │   16x2 LCD  │  │   CAN       │    │ │
│  │  │ Controller  │◄─┤   Display   │  │ Transceiver │    │ │
│  │  │             │  │   (I2C)     │  │  (MCP2551)  │    │ │
│  │  └─────────────┘  └─────────────┘  └─────────────┘    │ │
│  │         │                │                │            │ │
│  │  ┌─────────────┐  ┌─────────────┐  ┌─────────────┐    │ │
│  │  │   Buttons   │  │   Power     │  │   Serial    │    │ │
│  │  │   (GPIO)    │  │   Supply    │  │   Debug     │    │ │
│  │  └─────────────┘  └─────────────┘  └─────────────┘    │ │
│  └─────────────────────────────────────────────────────────┘ │
└─────────────────────────────────────────────────────────────┘
```

### **System Specifications**
- **Microcontroller**: ESP32 (240MHz, 320KB RAM, 4MB Flash)
- **Display**: 16x2 I2C LCD (HD44780 compatible)
- **Communication**: I2C (LCD), UART (Debug), CAN (Vehicle)
- **Power**: 5V/3.3V operation
- **Operating Temperature**: -40°C to +85°C
- **Update Rate**: 100Hz (10ms cycle)

---

## 🔌 Hardware Integration

### **1. Power Supply Requirements**

#### **Power Specifications**
```
Input Voltage: 5V ±5% (4.75V - 5.25V)
Current Draw: 150mA typical, 200mA maximum
Power Consumption: 0.75W typical, 1W maximum
```

#### **Power Supply Design**
```c
// Recommended power supply circuit
┌─────────┐    ┌─────────┐    ┌─────────┐
│  12V    │───▶│ 5V Reg  │───▶│  ESP32  │
│ Vehicle │    │ LM7805  │    │ 3.3V    │
│ Battery │    │         │    │ Internal│
└─────────┘    └─────────┘    └─────────┘
                       │
                       ▼
                ┌─────────┐
                │   LCD   │
                │   5V    │
                └─────────┘
```

#### **Power Filtering**
```c
// Add capacitors for stability
5V ──┬── 100µF ──┬── 0.1µF ── ESP32
     │            │
     └── 10µF ────┴── 0.01µF ── LCD
```

### **2. Physical Mounting**

#### **Enclosure Requirements**
- **Material**: ABS/PC plastic or aluminum
- **Protection**: IP54 minimum (dust and splash resistant)
- **Mounting**: DIN rail or dashboard mounting
- **Dimensions**: 120mm x 80mm x 30mm (approximate)

#### **Mounting Considerations**
```
┌─────────────────────────────────┐
│         Dashboard Mount         │
├─────────────────────────────────┤
│  ┌─────────────┐  ┌─────────┐  │
│  │   LCD       │  │  ESP32  │  │
│  │   Display   │  │  Board  │  │
│  └─────────────┘  └─────────┘  │
│  ┌─────────────┐  ┌─────────┐  │
│  │   Buttons   │  │  CAN    │  │
│  │             │  │  Conn   │  │
│  └─────────────┘  └─────────┘  │
└─────────────────────────────────┘
```

### **3. Cable Management**

#### **Cable Specifications**
| Cable | Type | Length | Gauge | Purpose |
|-------|------|--------|-------|---------|
| Power | 2-core | 1m | 18AWG | 5V supply |
| I2C | 4-core | 0.3m | 24AWG | LCD communication |
| CAN | 2-core | 2m | 20AWG | Vehicle data |
| Serial | 3-core | 1m | 24AWG | Debug/configuration |

#### **Cable Routing**
```
┌─────────────┐
│   Vehicle   │
│   CAN Bus   │
└─────┬───────┘
      │
      ▼
┌─────────────┐    ┌─────────────┐
│   CAN       │    │   Power     │
│ Transceiver │    │   Supply    │
└─────┬───────┘    └─────┬───────┘
      │                  │
      └────────┬─────────┘
               │
               ▼
        ┌─────────────┐
        │   ESP32     │
        │   VDU       │
        └─────┬───────┘
              │
              ▼
        ┌─────────────┐
        │   16x2 LCD  │
        │   Display   │
        └─────────────┘
```

---

## 💻 Software Integration

### **1. Build System Integration**

#### **PlatformIO Integration**
```ini
# platformio.ini
[env:vehicle_integration]
platform = espressif32
board = esp32doit-devkit-v1
framework = espidf
build_flags = 
    -Iinclude
    -Os
    -DCONFIG_ESP32_DEFAULT_CPU_FREQ_240=1
    -DCAN_BUS_ENABLED=1
    -DVEHICLE_INTEGRATION=1
```

#### **CMake Integration**
```cmake
# CMakeLists.txt
idf_component_register(
    SRCS
        "src/main.c"
        "src/dashboard.c"
        "src/pins.c"
        "src/serial.c"
        "src/system_util.c"
        "src/can_bus.c"
    INCLUDE_DIRS
        "."
        "include"
        "lib/I2CLcd"
        "lib/VDUDisplay"
)
```

### **2. API Integration**

#### **Vehicle Data Interface**
```c
// vehicle_interface.h
typedef struct {
    float vehicle_speed;        // km/h
    float engine_rpm;           // RPM
    float engine_temp;          // °C
    float fuel_level;           // %
    float battery_voltage;      // V
    unsigned int gear_position; // 1-6
    bool engine_running;        // true/false
} vehicle_data_t;

// API functions
esp_err_t vehicle_data_init(void);
esp_err_t vehicle_data_read(vehicle_data_t *data);
esp_err_t vehicle_data_subscribe(void (*callback)(vehicle_data_t *));
```

#### **Integration Example**
```c
// main.c integration
#include "vehicle_interface.h"

void vehicle_data_callback(vehicle_data_t *data) {
    // Update dashboard with real vehicle data
    dashboard_data.speed = (unsigned int)data->vehicle_speed;
    dashboard_data.rpm = (unsigned int)data->engine_rpm;
    dashboard_data.temperature = (unsigned int)data->engine_temp;
    dashboard_data.fuel_level = (unsigned int)data->fuel_level;
}

void app_main(void) {
    // Initialize vehicle interface
    vehicle_data_init();
    vehicle_data_subscribe(vehicle_data_callback);
    
    // Rest of VDU initialization
    vdu_pins_init();
    lcd_i2c_init();
    dashboard_init();
    
    // Main loop
    while (1) {
        dashboard_check_buttons();
        dashboard_show_page(dashboard_get_current_page(), &dashboard_data);
        vTaskDelay(pdMS_TO_TICKS(10));
    }
}
```

### **3. Configuration Management**

#### **Configuration File**
```c
// config.h
#define VDU_CONFIG_VERSION "1.0.0"

// Hardware configuration
#define VDU_LCD_I2C_ADDR      0x27
#define VDU_CAN_BUS_SPEED     500000  // 500kbps
#define VDU_UPDATE_RATE_MS    10

// Vehicle-specific configuration
#define VDU_VEHICLE_TYPE      "TRUCK"
#define VDU_MAX_SPEED         120     // km/h
#define VDU_MAX_RPM           3000    // RPM
#define VDU_TEMP_WARNING      95      // °C
#define VDU_FUEL_WARNING      15      // %

// Display configuration
#define VDU_DISPLAY_TIMEOUT   30      // seconds
#define VDU_BRIGHTNESS_LEVEL  80      // %
#define VDU_AUTO_DIM_ENABLE   1
```

#### **Runtime Configuration**
```c
// Configuration storage in NVS
typedef struct {
    uint8_t lcd_brightness;
    uint8_t update_rate;
    uint16_t can_speed;
    uint8_t display_timeout;
    char vehicle_id[16];
} vdu_config_t;

esp_err_t vdu_config_load(vdu_config_t *config);
esp_err_t vdu_config_save(vdu_config_t *config);
esp_err_t vdu_config_reset(void);
```

---

## 🚗 CAN Bus Integration

### **1. CAN Bus Hardware**

#### **MCP2551 Transceiver**
```
┌─────────────┐
│   MCP2551   │
│ Transceiver │
├─────────────┤
│ VDD    VSS  │
│ TXD    RXD  │
│ TXCAN  RXCAN│
│ STBY   RS   │
└─────────────┘
```

#### **Pin Connections**
```c
// ESP32 to MCP2551 connections
#define CAN_TX_PIN          5    // ESP32 TX → MCP2551 TXD
#define CAN_RX_PIN          4    // ESP32 RX ← MCP2551 RXD
#define CAN_STBY_PIN        2    // ESP32 GPIO → MCP2551 STBY
```

### **2. CAN Bus Configuration**

#### **CAN Bus Setup**
```c
// can_bus.c
#include "driver/can.h"

void can_bus_init(void) {
    can_config_t can_config = {
        .mode = CAN_MODE_NORMAL,
        .tx_io = (gpio_num_t)CAN_TX_PIN,
        .rx_io = (gpio_num_t)CAN_RX_PIN,
        .clkout_io = CAN_IO_UNUSED,
        .bus_off_io = CAN_IO_UNUSED,
        .tx_queue_len = 10,
        .rx_queue_len = 10,
        .alerts_enabled = CAN_ALERT_NONE,
        .clkout_divider = 0
    };
    
    can_timing_config_t can_timing = {
        .brp = 2,
        .tseg_1 = 13,
        .tseg_2 = 2,
        .sjw = 1,
        .triple_sampling = false
    };
    
    can_driver_install(&can_config, &can_timing, NULL);
    can_start();
}
```

#### **CAN Message Handling**
```c
// CAN message definitions
#define CAN_ID_SPEED        0x100
#define CAN_ID_RPM          0x101
#define CAN_ID_TEMP         0x102
#define CAN_ID_FUEL         0x103
#define CAN_ID_BATTERY      0x104

void can_message_handler(can_message_t *message) {
    switch (message->identifier) {
        case CAN_ID_SPEED:
            vehicle_data.speed = (float)(message->data[0] << 8 | message->data[1]) / 10.0;
            break;
        case CAN_ID_RPM:
            vehicle_data.rpm = (float)(message->data[0] << 8 | message->data[1]);
            break;
        case CAN_ID_TEMP:
            vehicle_data.temperature = message->data[0];
            break;
        case CAN_ID_FUEL:
            vehicle_data.fuel_level = message->data[0];
            break;
        case CAN_ID_BATTERY:
            vehicle_data.battery_voltage = (float)message->data[0] / 10.0;
            break;
    }
}
```

### **3. CAN Bus Testing**

#### **Test Messages**
```c
// Send test messages
void can_send_test_messages(void) {
    can_message_t message;
    
    // Speed message
    message.identifier = CAN_ID_SPEED;
    message.data_length_code = 2;
    message.data[0] = 0x04;  // 100 km/h
    message.data[1] = 0x00;
    can_transmit(&message, pdMS_TO_TICKS(100));
    
    // RPM message
    message.identifier = CAN_ID_RPM;
    message.data_length_code = 2;
    message.data[0] = 0x0B;  // 2500 RPM
    message.data[1] = 0xB8;
    can_transmit(&message, pdMS_TO_TICKS(100));
}
```

---

## ⚡ Power Management

### **1. Power Requirements**

#### **Component Power Consumption**
| Component | Voltage | Current | Power |
|-----------|---------|---------|-------|
| ESP32 | 3.3V | 80mA | 264mW |
| LCD Display | 5V | 50mA | 250mW |
| CAN Transceiver | 5V | 20mA | 100mW |
| **Total** | **-** | **150mA** | **614mW** |

#### **Power Supply Design**
```c
// Power supply circuit
12V Vehicle ──┬── LM7805 ── 5V ──┬── ESP32 (3.3V internal)
              │                  │
              │                  ├── LCD Display
              │                  │
              │                  └── CAN Transceiver
              │
              └── LM317 ── 3.3V ── (backup)
```

### **2. Power Monitoring**

#### **Battery Voltage Monitoring**
```c
// Battery voltage monitoring
#define BATTERY_VOLTAGE_PIN  35

float read_battery_voltage(void) {
    int adc_value = adc1_get_raw(BATTERY_VOLTAGE_PIN);
    float voltage = (adc_value * 3.3) / 4095.0 * 3.0; // Voltage divider
    return voltage;
}

void check_power_status(void) {
    float battery_voltage = read_battery_voltage();
    
    if (battery_voltage < 10.5) {
        // Low battery warning
        lcd_i2c_set_cursor(0, 0);
        lcd_i2c_print("LOW BATTERY!");
        lcd_i2c_set_cursor(0, 1);
        lcd_i2c_print("VOLT: ");
        char buf[10];
        snprintf(buf, sizeof(buf), "%.1fV", battery_voltage);
        lcd_i2c_print(buf);
    }
}
```

### **3. Power Saving Features**

#### **Sleep Mode**
```c
// Power saving sleep mode
void enter_sleep_mode(void) {
    // Save current state
    vdu_config_save(&current_config);
    
    // Configure wake-up sources
    esp_sleep_enable_ext0_wakeup(GPIO_NUM_0, 0); // BOOT button
    esp_sleep_enable_timer_wakeup(30 * 1000000); // 30 seconds
    
    // Enter deep sleep
    esp_deep_sleep_start();
}

void wake_up_handler(void) {
    // Restore state
    vdu_config_load(&current_config);
    
    // Reinitialize display
    lcd_i2c_init();
    dashboard_init();
}
```

---

## 🌡️ Environmental Considerations

### **1. Temperature Management**

#### **Operating Temperature Range**
- **Operating**: -40°C to +85°C
- **Storage**: -55°C to +125°C
- **Humidity**: 5% to 95% (non-condensing)

#### **Thermal Management**
```c
// Temperature monitoring
#define TEMP_SENSOR_PIN      34

float read_internal_temperature(void) {
    // Read internal temperature sensor
    return esp_timer_get_time() / 1000000.0; // Simplified
}

void thermal_management(void) {
    float temp = read_internal_temperature();
    
    if (temp > 70) {
        // Reduce CPU frequency
        esp_pm_config_esp32_t pm_config = {
            .max_freq_mhz = 160,
            .min_freq_mhz = 80,
        };
        esp_pm_configure(&pm_config);
    }
}
```

### **2. Vibration and Shock**

#### **Mechanical Specifications**
- **Vibration**: 10-55Hz, 1.5mm amplitude
- **Shock**: 50g, 11ms duration
- **Mounting**: Anti-vibration mounts recommended

#### **Vibration Protection**
```c
// Vibration detection
#define ACCELEROMETER_PIN    36

bool detect_vibration(void) {
    int accel_value = analogRead(ACCELEROMETER_PIN);
    return (accel_value > VIBRATION_THRESHOLD);
}

void vibration_protection(void) {
    if (detect_vibration()) {
        // Reduce update rate during vibration
        vTaskDelay(pdMS_TO_TICKS(50)); // 50ms instead of 10ms
    }
}
```

### **3. EMI/EMC Considerations**

#### **Shielding Requirements**
- **Enclosure**: Metal or conductive plastic
- **Cables**: Shielded cables for CAN bus
- **Grounding**: Proper ground connection

#### **EMI Protection**
```c
// EMI filtering
#define EMI_FILTER_PIN       37

void emi_protection_init(void) {
    // Configure EMI filter
    gpio_config_t emi_config = {
        .pin_bit_mask = (1ULL << EMI_FILTER_PIN),
        .mode = GPIO_MODE_INPUT,
        .pull_up_en = 1,
    };
    gpio_config(&emi_config);
}
```

---

## 🧪 Testing & Validation

### **1. Hardware Testing**

#### **Component Testing Checklist**
- [ ] ESP32 boot and initialization
- [ ] LCD display functionality
- [ ] Button input response
- [ ] CAN bus communication
- [ ] Power supply stability
- [ ] Temperature monitoring

#### **Test Procedures**
```c
// Hardware test routine
void hardware_test_routine(void) {
    printf("Starting hardware test...\n");
    
    // Test LCD
    lcd_i2c_clear();
    lcd_i2c_print("HARDWARE TEST");
    vTaskDelay(pdMS_TO_TICKS(1000));
    
    // Test buttons
    printf("Press BOOT button to continue...\n");
    while (gpio_get_level(VDU_BTN1) == 1) {
        vTaskDelay(pdMS_TO_TICKS(100));
    }
    
    // Test CAN bus
    can_send_test_messages();
    
    printf("Hardware test complete!\n");
}
```

### **2. Software Testing**

#### **Unit Tests**
```c
// Unit test framework
void test_dashboard_functions(void) {
    dashboard_data_t test_data = {0};
    
    // Test data update
    test_data.speed = 100;
    test_data.rpm = 2500;
    dashboard_update_data(&test_data);
    
    // Verify calculations
    assert(test_data.fuel_level > 0);
    assert(test_data.temperature > 0);
    
    printf("Dashboard tests passed!\n");
}
```

#### **Integration Tests**
```c
// Integration test
void integration_test(void) {
    // Test complete system
    vdu_pins_init();
    lcd_i2c_init();
    dashboard_init();
    can_bus_init();
    
    // Run for 60 seconds
    for (int i = 0; i < 6000; i++) {
        dashboard_check_buttons();
        dashboard_show_page(dashboard_get_current_page(), &dashboard_data);
        vTaskDelay(pdMS_TO_TICKS(10));
    }
    
    printf("Integration test complete!\n");
}
```

### **3. Vehicle Integration Testing**

#### **CAN Bus Testing**
```c
// CAN bus validation
void can_bus_validation(void) {
    can_message_t test_message;
    
    // Send test messages
    for (int i = 0; i < 100; i++) {
        test_message.identifier = 0x100 + i;
        test_message.data_length_code = 2;
        test_message.data[0] = i;
        test_message.data[1] = i + 1;
        
        esp_err_t result = can_transmit(&test_message, pdMS_TO_TICKS(100));
        if (result != ESP_OK) {
            printf("CAN transmission failed: %s\n", esp_err_to_name(result));
        }
    }
}
```

---

## 🚀 Deployment Guide

### **1. Pre-Deployment Checklist**

#### **Hardware Checklist**
- [ ] All components properly mounted
- [ ] Power supply tested and stable
- [ ] Cables properly routed and secured
- [ ] Enclosure sealed and protected
- [ ] Ground connections verified

#### **Software Checklist**
- [ ] Firmware compiled and tested
- [ ] Configuration parameters set
- [ ] CAN bus addresses configured
- [ ] Error handling implemented
- [ ] Logging enabled

### **2. Installation Procedure**

#### **Step 1: Hardware Installation**
```bash
# 1. Mount VDU in vehicle
# 2. Connect power supply
# 3. Connect CAN bus
# 4. Connect LCD display
# 5. Verify all connections
```

#### **Step 2: Software Installation**
```bash
# 1. Upload firmware
pio run -t upload

# 2. Configure system
pio device monitor
# Enter configuration mode and set parameters

# 3. Test system
# Run hardware test routine
```

#### **Step 3: Vehicle Integration**
```bash
# 1. Connect to vehicle CAN bus
# 2. Verify CAN messages
# 3. Test all dashboard pages
# 4. Verify button functionality
# 5. Test power management
```

### **3. Commissioning**

#### **System Commissioning**
```c
// Commissioning routine
void system_commissioning(void) {
    printf("Starting system commissioning...\n");
    
    // 1. Hardware verification
    hardware_test_routine();
    
    // 2. CAN bus verification
    can_bus_validation();
    
    // 3. Display verification
    display_test_routine();
    
    // 4. Power management test
    power_management_test();
    
    // 5. Save configuration
    vdu_config_save(&commissioned_config);
    
    printf("System commissioning complete!\n");
}
```

---

## 🔧 Maintenance & Support

### **1. Preventive Maintenance**

#### **Maintenance Schedule**
| Component | Interval | Action |
|-----------|----------|--------|
| LCD Display | 6 months | Clean display surface |
| Buttons | 12 months | Check button response |
| CAN Bus | 12 months | Verify CAN communication |
| Power Supply | 12 months | Check voltage levels |
| Firmware | As needed | Update to latest version |

#### **Maintenance Procedures**
```c
// Maintenance mode
void maintenance_mode(void) {
    printf("Entering maintenance mode...\n");
    
    // Run diagnostics
    system_diagnostics();
    
    // Check component health
    check_lcd_health();
    check_button_health();
    check_can_health();
    check_power_health();
    
    // Generate maintenance report
    generate_maintenance_report();
}
```

### **2. Troubleshooting**

#### **Common Issues and Solutions**

| Issue | Symptoms | Solution |
|-------|----------|----------|
| LCD not working | No display | Check I2C address, wiring |
| Button not responding | No page change | Check GPIO configuration |
| CAN bus errors | No vehicle data | Check CAN wiring, termination |
| Power issues | System reset | Check power supply, voltage |
| Communication errors | No serial output | Check USB connection, drivers |

#### **Diagnostic Tools**
```c
// Diagnostic routine
void system_diagnostics(void) {
    printf("=== System Diagnostics ===\n");
    
    // Memory usage
    printf("Free heap: %d bytes\n", esp_get_free_heap_size());
    printf("Minimum free heap: %d bytes\n", esp_get_minimum_free_heap_size());
    
    // CPU usage
    printf("CPU frequency: %d MHz\n", esp_clk_cpu_freq() / 1000000);
    
    // Temperature
    printf("Temperature: %.1f°C\n", read_internal_temperature());
    
    // Power status
    printf("Battery voltage: %.1fV\n", read_battery_voltage());
    
    // CAN bus status
    can_status_t can_status;
    can_get_status(&can_status);
    printf("CAN bus errors: %d\n", can_status.rx_error_counter);
}
```

### **3. Support Procedures**

#### **Support Contact Information**
- **Documentation**: https://github.com/shantanuk47/VDU_ESP32/docs
- **Issue Tracking**: https://github.com/shantanuk47/VDU_ESP32/issues

#### **Support Request Template**
```
Subject: VDU_ESP32 Support Request

System Information:
- Firmware Version: 
- Hardware Version: 
- Vehicle Type: 
- Installation Date: 

Issue Description:
- What happened: 
- When did it happen: 
- What was expected: 
- What actually happened: 

Diagnostic Information:
- Error messages: 
- System logs: 
- Recent changes: 

Contact Information:
- Name: 
- Email: 
- Phone: 
```

---

## 📊 Performance Monitoring

### **1. System Performance Metrics**

#### **Key Performance Indicators**
- **Response Time**: < 10ms for button press
- **Update Rate**: 100Hz display refresh
- **Memory Usage**: < 15% of available RAM
- **CPU Usage**: < 30% average
- **Power Consumption**: < 1W typical

#### **Performance Monitoring**
```c
// Performance monitoring
typedef struct {
    uint32_t button_response_time;
    uint32_t display_update_time;
    uint32_t can_message_latency;
    uint32_t memory_usage;
    uint32_t cpu_usage;
} performance_metrics_t;

void monitor_performance(void) {
    performance_metrics_t metrics;
    
    // Measure button response time
    unsigned long start_time = xTaskGetTickCount();
    dashboard_check_buttons();
    metrics.button_response_time = (xTaskGetTickCount() - start_time) * portTICK_PERIOD_MS;
    
    // Measure display update time
    start_time = xTaskGetTickCount();
    dashboard_show_page(dashboard_get_current_page(), &dashboard_data);
    metrics.display_update_time = (xTaskGetTickCount() - start_time) * portTICK_PERIOD_MS;
    
    // Log performance metrics
    log_performance_metrics(&metrics);
}
```

### **2. Data Logging**

#### **Logging Configuration**
```c
// Logging system
typedef enum {
    LOG_LEVEL_ERROR = 0,
    LOG_LEVEL_WARN,
    LOG_LEVEL_INFO,
    LOG_LEVEL_DEBUG
} log_level_t;

void log_message(log_level_t level, const char *message) {
    unsigned long timestamp = xTaskGetTickCount() * portTICK_PERIOD_MS;
    printf("[%lu][%d] %s\n", timestamp, level, message);
}
```

---

**Last Updated**: August 1, 2025  
**Version**: 1.0.0  
**Author**: Shantanu Kumar  
**Document Type**: System Integrator Manual 