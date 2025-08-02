@echo off
REM ========================================
REM ESP32 Connection Test
REM ========================================

echo ========================================
echo ESP32 Connection Test
echo ========================================
echo.

cd /d "%~dp0"

echo [1/4] Checking PlatformIO installation...
pio --version
IF ERRORLEVEL 1 (
    echo [ERROR] PlatformIO not found. Please install PlatformIO Core.
    pause
    exit /b 1
)
echo [OK] PlatformIO is installed.
echo.

echo [2/4] Checking for ESP32 devices...
echo.
echo Available devices:
pio device list
echo.

echo [3/4] Testing ESP32 connection...
echo.
echo Attempting to connect to ESP32...
echo If this fails, try:
echo 1. Hold BOOT button + press RESET, then release BOOT
echo 2. Check USB cable and port
echo 3. Install/update USB drivers
echo.

REM Try to get device info
pio device monitor --baud 115200 --timeout 5
echo.

echo [4/4] Testing upload functionality...
echo.
echo Building project...
pio run
IF ERRORLEVEL 1 (
    echo [ERROR] Build failed!
    pause
    exit /b 1
)
echo [OK] Build successful!
echo.

echo Ready to test upload.
echo.
echo To test upload manually:
echo 1. Put ESP32 in bootloader mode (BOOT + RESET)
echo 2. Run: pio run -t upload
echo 3. Or use VDU Manager GUI
echo.
pause 