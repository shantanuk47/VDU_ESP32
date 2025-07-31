@echo off
REM ========================================
REM Build and upload ESP32 project with PlatformIO
REM Author: Shantanu Kumar
REM Date: 2025-07-31
REM ========================================

REM Change to the directory where this script is located (your project root)
cd /d "%~dp0"

echo Building project...
pio run
IF ERRORLEVEL 1 (
    echo.
    echo [ERROR] Build failed. Aborting upload.
    pause
    exit /b 1
)

echo.
echo Uploading to ESP32...
pio run -t upload
IF ERRORLEVEL 1 (
    echo.
    echo [ERROR] Upload failed.
    pause
    exit /b 1
)

echo.
echo [SUCCESS] Build and upload complete!
pause
