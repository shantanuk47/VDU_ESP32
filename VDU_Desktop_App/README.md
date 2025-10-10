# VDU Desktop Application

This folder contains the desktop GUI application for managing your VDU ESP32 project.

## 📁 Contents

- **`VDU_Manager.py`** - Main GUI application
- **`build_exe.bat`** - Build script for creating standalone executable
- **`requirements.txt`** - Python dependencies
- **`GUI_README.md`** - Detailed usage guide

## 🚀 Quick Start

### Option 1: Run from Source
```bash
# Navigate to this folder
cd VDU_Desktop_App

# Install dependencies
pip install -r requirements.txt

# Run the application
python VDU_Manager.py
```

### Option 2: Build Executable
```bash
# Navigate to this folder
cd VDU_Desktop_App

# Run build script
./build_exe.bat

# Run the generated executable
./dist/VDU Manager.exe
```

## 🎯 Features

- **📤 Firmware Management** - Upload, build, and clean ESP32 firmware
- **📡 Serial Monitoring** - Real-time communication with ESP32
- **🕐 RTC Configuration** - Set DS3231 RTC time with code generation
- **📋 Log Management** - Save and view serial logs
- **🎨 Professional GUI** - Modern, user-friendly interface

## 📊 System Requirements

- **Python 3.6+** (for source version)
- **Windows 7/8/10/11** (for executable)
- **4 GB RAM** minimum
- **100 MB** free disk space

## 🔧 Dependencies

- **pyserial** - Serial communication
- **tkinter** - GUI framework (built-in)
- **PyInstaller** - Executable creation

## 📖 Documentation

See `GUI_README.md` for detailed usage instructions and troubleshooting.

---

**Note:** This application manages the main VDU ESP32 project located in the parent directory. 