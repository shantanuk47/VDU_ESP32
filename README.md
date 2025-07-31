
# VDU_ESP32
**Vehicle Display Unit (VDU) for ESP32**  
This project implements a Vehicle Display Unit using the ESP32 microcontroller platform. The software is designed for robust, real-time vehicle data visualization and control, targeting both development boards and custom hardware.

## 📟 Features

- **16x2 LCD display** via I2C
- CAN transceiver support (e.g., MCP2551)
- Real-time vehicle telemetry display (current, voltage, temperatures, drive mode, etc.)
- Modular code structure for easy feature expansion
- Hardware abstraction layer for portability

## 🧰 Hardware Used

| Component       | Description              |
|----------------|--------------------------|
| ESP32           | Microcontroller          |
| CJMCU-2551      | MCP2551 CAN Transceiver  |
| 16x2 LCD (I2C)  | Display interface        |

## 📦 Setup Instructions

1. Clone this repo:
   ```bash
   git clone https://github.com/shantanuk47/VDU_ESP32.git
   cd VDU_ESP32
   ```
2. Open in [PlatformIO](https://platformio.org/) (VS Code recommended)
3. Connect your ESP32 board via USB.
4. Build and upload the firmware using PlatformIO:
   ```bash
   pio run --target upload
   ```
5. Update `platformio.ini` as needed for your ESP32 variant.

## Project Overview

- **Platform:** ESP32
- **Framework:** PlatformIO (ESP-IDF/Arduino, to be updated as needed)
- **Language:** C

## Directory Structure

```
VDU_ESP32/
├── config/ # Configuration headers (pin definitions, hardware settings)
├── include/ # Project-wide header files (public APIs)
├── lib/ # Custom driver libraries (I2C LCD, VDU display, etc.)
│ ├── I2CLcd/ # I2C LCD driver (pure C)
│ └── VDUDisplay/ # Dashboard/vehicle display logic
├── src/ # Main source files (main.c, pins.c, serial.c, etc.)
├── test/ # Unit and integration test cases
├── README.md # Project overview and documentation
├── platformio.ini # PlatformIO build and environment config
└── .gitignore # Git version control ignores
```

## License

This project is licensed under the MIT License.  
See [LICENSE](LICENSE) for more details.

## Related Projects

- [VDU_ESP32-microPython-](https://github.com/shantanuk47/VDU_ESP32-microPython-): MicroPython-based version of this project

## Author

Developed and maintained by [Shantanu Kumar](https://github.com/shantanuk47)

