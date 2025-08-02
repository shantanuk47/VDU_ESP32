# VDU ESP32 Manager GUI

A professional GUI application for managing your VDU ESP32 project with upload, monitoring, and configuration capabilities.

## 🚀 Features

### 📤 Firmware Management
- **Upload Firmware** - Compile and upload to ESP32
- **Build Only** - Compile without uploading
- **Clean Build** - Remove build artifacts

### 📡 Serial Communication
- **Real-time Monitoring** - Live serial data display
- **Port Auto-detection** - Automatic ESP32 port detection
- **Configurable Baud Rate** - Support for common baud rates
- **Log Saving** - Save serial output to file

### 🕐 RTC Configuration
- **Time Setting** - Set DS3231 RTC time
- **Code Generation** - Automatic C code generation
- **Validation** - Input validation for time/date

### 🎨 Professional Interface
- **Modern GUI** - Clean, professional appearance
- **Status Updates** - Real-time operation status
- **Error Handling** - Comprehensive error messages
- **Cross-platform** - Works on Windows, macOS, Linux

## 📦 Installation

### Option 1: Run from Source
```bash
# Install dependencies
pip install -r requirements.txt

# Run the application
python VDU_Manager.py
```

### Option 2: Use Executable
```bash
# Build executable
./build_exe.bat

# Run the generated executable
./dist/VDU Manager.exe
```

## 🎯 Usage Guide

### 1. Firmware Upload
1. Click **"📤 Upload Firmware"**
2. Wait for compilation and upload
3. Check status bar for progress
4. Monitor serial output for results

### 2. Serial Monitoring
1. Select your ESP32 port from dropdown
2. Choose baud rate (default: 115200)
3. Click **"📡 Start Monitor"**
4. View real-time serial data
5. Click **"⏹️ Stop Monitor"** when done

### 3. RTC Time Setting
1. Click **"🕐 Set RTC Time"**
2. Enter desired time and date
3. Click **"Apply"**
4. Copy generated C code to your project
5. Upload firmware to apply changes

### 4. Log Management
- **Clear** - Clear current monitor output
- **Save Log** - Save serial output to file
- **Auto-refresh** - Ports refresh every 5 seconds

## 🔧 Configuration

### Serial Port Settings
- **Port Selection** - Automatically detects ESP32
- **Baud Rate** - 9600, 19200, 38400, 57600, 115200
- **Auto-refresh** - Updates available ports

### Project Information
- **Project Path** - Current working directory
- **Hardware Info** - ESP32 DevKit V1, 16x2 LCD, DS3231 RTC

## 🛠️ Troubleshooting

### Common Issues

#### PlatformIO Not Found
```
❌ PlatformIO not found - install PlatformIO CLI
```
**Solution:** Install PlatformIO CLI
```bash
pip install platformio
```

#### Serial Port Not Available
```
❌ Serial monitor error: [Errno 2] No such file or directory
```
**Solution:** 
1. Check USB connection
2. Install ESP32 drivers
3. Try different USB cable

#### Upload Timeout
```
❌ Upload timeout - check connection
```
**Solution:**
1. Hold BOOT button during upload
2. Check USB connection
3. Try different USB port

### Error Messages
- **Green text** - Success messages
- **Red text** - Error messages
- **Status bar** - Current operation status

## 📁 File Structure

```
VDU_ESP32/
├── VDU_Manager.py          # Main GUI application
├── build_exe.bat           # Build script for executable
├── requirements.txt        # Python dependencies
├── GUI_README.md          # This file
├── dist/                  # Generated executable (after build)
│   └── VDU Manager.exe
└── build/                 # Build artifacts (after build)
```

## 🔄 Building Executable

### Automatic Build
```bash
# Run the build script
./build_exe.bat
```

### Manual Build
```bash
# Install PyInstaller
pip install pyinstaller

# Build executable
pyinstaller --onefile --windowed --name="VDU Manager" VDU_Manager.py
```

### Build Options
- **--onefile** - Single executable file
- **--windowed** - No console window
- **--name** - Custom executable name
- **--icon** - Add custom icon (optional)

## 🌐 Distribution

### Single File Distribution
- **File:** `dist/VDU Manager.exe`
- **Size:** ~25-50 MB
- **Requirements:** None (self-contained)
- **Platform:** Windows

### Cross-Platform
- **Windows:** `.exe` file
- **macOS:** `.app` bundle
- **Linux:** Binary executable

## 📊 System Requirements

### Minimum Requirements
- **OS:** Windows 7/8/10/11, macOS 10.12+, Linux
- **RAM:** 4 GB
- **Storage:** 100 MB free space
- **USB:** USB 2.0 port for ESP32

### Recommended
- **OS:** Windows 10/11
- **RAM:** 8 GB
- **Storage:** 1 GB free space
- **USB:** USB 3.0 port

## 🤝 Support

### Getting Help
1. Check the troubleshooting section
2. Review error messages in the GUI
3. Check serial monitor output
4. Verify hardware connections

### Common Commands
```bash
# Check Python version
python --version

# Check installed packages
pip list

# Test serial communication
python -c "import serial; print('Serial OK')"

# Test GUI
python -c "import tkinter; print('GUI OK')"
```

## 🔄 Updates

### Updating the Application
1. Download new version
2. Replace `VDU_Manager.py`
3. Rebuild executable if needed
4. Test functionality

### Version History
- **v1.0** - Initial release with basic functionality
- **Features:** Upload, monitor, RTC setting, log saving

---

**Happy coding with your VDU ESP32 project!** 🚗✨ 