
# VDU_ESP32
**Vehicle Display Unit (VDU) for ESP32**  
A comprehensive vehicle dashboard system using ESP32 with real-time multi-page display, CAN bus communication, RTC time display, and desktop management application. Perfect for automotive projects, vehicle monitoring, and dashboard development.

## 🚗 Features

### **Core Functionality**
- **16x2 I2C LCD Display** - High-quality vehicle data visualization
- **Multi-Page Dashboard** - 5 different information pages with date/time display
- **BOOT Button Navigation** - Easy page switching with built-in button
- **Real-time Data Simulation** - Speed, odometer, RPM, temperature, fuel, trip data
- **CAN Bus Communication** - MCP2551 transceiver with 500kbps communication
- **RTC Time Display** - DS3231 real-time clock with date/time formatting
- **Responsive UI** - Multi-rate system (CAN: 10Hz, System: 100Hz, Display: 2Hz)

### **Dashboard Pages**
1. **Speed Page** - Vehicle speed (100-120 km/h) & odometer (XXXXXX.X format)
2. **Engine Page** - RPM & engine temperature
3. **Fuel Page** - Fuel level percentage & range
4. **Trip Page** - Trip distance & time
5. **Compact Page** - All key data in one view with date/time display

### **Technical Features**
- **ESP-IDF Framework** - Professional-grade development
- **FreeRTOS Integration** - Real-time task management
- **I2C Communication** - Reliable LCD and RTC interface
- **CAN Bus Integration** - MCP2551 transceiver with TWAI driver
- **GPIO Button Handling** - Debounced button input
- **Serial Command Interface** - RTC time setting and system configuration
- **Modular Architecture** - Easy to extend and modify

## 🧰 Hardware Requirements

| Component | Description | Pin Connection |
|-----------|-------------|----------------|
| **ESP32 Development Board** | Main microcontroller | - |
| **16x2 I2C LCD Display** | Vehicle data display | SDA: GPIO 21, SCL: GPIO 22 |
| **DS3231 RTC Module** | Real-time clock | SDA: GPIO 21, SCL: GPIO 22 (shared I2C) |
| **MCP2551 CAN Transceiver** | CAN bus communication | CTX: GPIO 5, CRX: GPIO 4 |
| **BOOT Button** | Page navigation (built-in) | GPIO 0 |
| **USB Cable** | Power and programming | - |

## 📦 Quick Start

### **1. Setup Environment**
```bash
git clone https://github.com/shantanuk47/VDU_ESP32.git
cd VDU_ESP32
```

### **2. Hardware Connection**
- Connect I2C LCD and RTC to ESP32:
  - **SDA** → GPIO 21
  - **SCL** → GPIO 22
  - **VCC** → 5V
  - **GND** → GND
- Connect CAN transceiver:
  - **CTX** → GPIO 5
  - **CRX** → GPIO 4
  - **VCC** → 5V
  - **GND** → GND

### **3. Build & Upload**
```bash
# Using PlatformIO
pio run -t upload

# Monitor serial output
pio device monitor
```

### **4. Operation**
- **Power On**: LCD shows "VDU Ready" and "Press BOOT"
- **Navigation**: Press BOOT button to cycle through dashboard pages
- **Data**: Real-time simulated vehicle data updates continuously
- **Time Display**: Date and time shown in format "D: DD MMM YYYY T: HH:MM:SS AM/PM"

## 🖥️ Desktop Application

A professional GUI application is available for easy project management:

### **Features**
- **📤 Firmware Upload** - One-click ESP32 programming
- **📡 Serial Monitoring** - Real-time communication
- **🕐 RTC Time Setting** - Set DS3231 time via GUI (no more reboots!)
- **📋 Log Management** - Save and view serial logs
- **🔄 Port Management** - Automatic port detection and configuration

### **Quick Start**
```bash
# Navigate to desktop app folder
cd VDU_Desktop_App

# Install dependencies
pip install -r requirements.txt

# Run GUI application
python VDU_Manager.py

# Or build standalone executable
./build_exe.bat
```

**📖 See `VDU_Desktop_App/README.md` for detailed instructions.**

## 🎮 User Interface

### **Button Controls**
- **BOOT Button**: Navigate between dashboard pages
- **Page Cycle**: Speed → Engine → Fuel → Trip → Compact → Speed

### **Display Information**
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

## 📁 Project Structure

```
VDU_ESP32/
├── include/                    # Header files
│   ├── pins.h                 # Pin definitions
│   ├── dashboard.h            # Dashboard interface
│   ├── date_time.h            # Date/time handling
│   └── ...
├── lib/                       # Custom libraries
│   ├── I2CLcd/               # I2C LCD driver
│   │   ├── lcd_i2c.c         # LCD implementation
│   │   └── lcd_i2c.h         # LCD interface
│   ├── VDUDisplay/           # Display logic
│   │   ├── vdu_display.c     # Display functions
│   │   └── vdu_display.h     # Display interface
│   ├── CANBus/               # CAN bus communication
│   │   ├── can.c             # CAN implementation
│   │   └── can.h             # CAN interface
│   └── DS3231/               # RTC module
│       ├── rtc.c             # RTC implementation
│       └── rtc.h             # RTC interface
├── src/                       # Main source files
│   ├── main.c                # Application entry point
│   ├── dashboard.c           # Dashboard logic
│   ├── date_time.c           # Date/time handling
│   ├── serial.c              # Serial command interface
│   └── ...
├── VDU_Desktop_App/          # Desktop GUI Application
│   ├── VDU_Manager.py        # Main GUI application
│   ├── build_exe.bat         # Build script for executable
│   ├── requirements.txt      # Python dependencies
│   ├── GUI_README.md         # Detailed GUI documentation
│   └── README.md             # Desktop app overview
├── docs/                     # Documentation
│   ├── README.md             # Main project documentation
│   ├── QUICK_REFERENCE.md    # Quick setup guide
│   ├── SYSTEM_INTEGRATOR_MANUAL.md
│   └── PROGRAMMER_MANUAL.md
├── platformio.ini            # PlatformIO configuration
└── upload.bat                # Quick upload script
```

## ⚙️ Configuration

### **Pin Definitions** (`include/pins.h`)
```c
#define VDU_LCD_I2C_SDA    21    // I2C SDA pin (shared with RTC)
#define VDU_LCD_I2C_SCL    22    // I2C SCL pin (shared with RTC)
#define VDU_BTN1           0     // BOOT button
#define VDU_BTN2           13    // Future external button
#define VDU_CAN_TX         5     // MCP2551 CTX pin
#define VDU_CAN_RX         4     // MCP2551 CRX pin
```

### **LCD Configuration** (`lib/I2CLcd/lcd_i2c.h`)
```c
#define LCD_I2C_ADDR      0x27   // I2C address (0x27 or 0x3F)
#define LCD_COLS          16     // Display columns
#define LCD_ROWS          2      // Display rows
```

### **RTC Configuration** (`lib/DS3231/rtc.h`)
```c
#define VDU_RTC_I2C_ADDR  0x68   // DS3231 I2C address
#define I2C_MASTER_FREQ_HZ 50000 // 50kHz I2C speed
```

## 🔧 Customization

### **Adding New Dashboard Pages**
1. Add page enum in `include/dashboard.h`
2. Implement display logic in `src/dashboard.c`
3. Update page count and navigation

### **Modifying Data Simulation**
Edit `dashboard_update_data()` in `src/dashboard.c`:
```c
data->rpm = data->speed * 25;           // RPM calculation
data->temperature = 80 + (data->speed % 20);  // Temperature
data->fuel_level = 65 + (data->speed % 30);   // Fuel level
```

### **Changing Display Format**
Modify `snprintf` format strings in `dashboard_show_page()`:
```c
snprintf(buf, sizeof(buf), "ODO:%08.1f KM  ", data->odometer);
```

### **Setting RTC Time**
Use the desktop application or serial commands:
```bash
# Via serial monitor
SET_TIME 2025 08 02 14 30 25

# Via desktop GUI
# Use the "Set RTC Time" button in VDU Manager
```

## 🚀 Current Features

### **Implemented Features**
- **CAN Bus Integration** - MCP2551 transceiver with 500kbps communication
- **Real-time CAN Transmission** - Sends vehicle data (ID 0x301) every 100ms
- **RTC Time Display** - DS3231 real-time clock with date/time formatting
- **Multi-page Dashboard** - 5 different display pages with navigation
- **Button Navigation** - Added button for page cycling (boot button temporary)
- **Serial Command Interface** - INFO, SET_TIME, and other commands
- **Desktop Management App** - Python GUI for easy project management
- **Version Management** - Smart version display system
- **Multi-rate System** - CAN: 10Hz, System: 100Hz, Display: 2Hz

### **Hardware Integration**
- **CAN Bus**: MCP2551 transceiver on GPIO 4/5
- **LCD Display**: 16x2 I2C LCD on GPIO 21/22
- **RTC Module**: DS3231 on GPIO 21/22 (shared I2C)
- **Buttons**: BOOT button (GPIO 0) for navigation
- **Serial Interface**: USB debugging and configuration

### **Data Flow**
```
Vehicle CAN Bus → MCP2551 → ESP32 → LCD Display
                ↓
            Real-time data transmission (0x301)
            - Speed, RPM, Temperature, Fuel Level
            - Status bits and checksum validation
            - Date/time from DS3231 RTC
```

## 🐛 Troubleshooting

### **LCD Not Working**
- Check I2C address (try 0x27 or 0x3F)
- Verify wiring connections
- Check power supply (5V required)

### **RTC Not Working**
- Check I2C address (should be 0x68)
- Verify shared I2C bus with LCD
- Use desktop app to set time
- Check serial output for RTC errors

### **CAN Bus Issues**
- Verify MCP2551 connections (CTX: GPIO 5, CRX: GPIO 4)
- Check CAN bus termination (120Ω resistors)
- Monitor serial output for CAN errors

### **Button Not Responding**
- Ensure BOOT button is properly connected
- Check GPIO configuration in `pins.h`
- Monitor serial output for button events

### **Desktop App Issues**
- Check serial port availability
- Ensure ESP32 is not in use by other applications
- Verify Python dependencies are installed
- Check firewall settings for COM port access

### **Build Issues**
- Verify PlatformIO installation
- Check ESP32 board selection
- Ensure all dependencies are installed

## 📊 Performance

- **Update Rate**: Multi-rate system
  - CAN: 100ms (10 Hz) - Real-time vehicle data
  - System: 10ms (100 Hz) - Button handling and data processing
  - Display: 500ms (2 Hz) - LCD refresh rate
- **CAN Transmission**: 100ms intervals (ID 0x301)
- **Memory Usage**: ~11KB RAM (3.4%)
- **Flash Usage**: ~264KB (25.2%)
- **CPU Frequency**: 240MHz
- **I2C Speed**: 50kHz (stable operation)
- **CAN Speed**: 500kbps (MCP2551 transceiver)

## 🤝 Contributing

1. Fork the repository
2. Create a feature branch
3. Make your changes
4. Test thoroughly
5. Submit a pull request

## 📄 License

This project is licensed under the MIT License.  
See [LICENSE](LICENSE) for details.

## 👨‍💻 Author

**Shantanu Kumar**  
- GitHub: [@shantanuk47](https://github.com/shantanuk47)
- Project: [VDU_ESP32](https://github.com/shantanuk47/VDU_ESP32)

## 🔗 Related Projects

- [VDU_ESP32-microPython](https://github.com/shantanuk47/VDU_ESP32-microPython-): MicroPython version
- [ESP32 CAN Bus Projects](https://github.com/shantanuk47): Additional ESP32 projects

---

**⭐ Star this repository if you find it useful!**

