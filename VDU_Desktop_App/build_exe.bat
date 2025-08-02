@echo off
title VDU Manager - Build Executable
color 0A

echo ========================================
echo    VDU ESP32 Manager - Build Script
echo ========================================
echo.

REM Check if Python is installed
python --version >nul 2>&1
if errorlevel 1 (
    echo ❌ Python not found! Please install Python 3.6+
    pause
    exit /b 1
)

echo ✅ Python found
echo.

REM Check if required packages are installed
echo Checking required packages...

REM Check PyInstaller
python -c "import PyInstaller" >nul 2>&1
if errorlevel 1 (
    echo 📦 Installing PyInstaller...
    pip install pyinstaller
    if errorlevel 1 (
        echo ❌ Failed to install PyInstaller
        pause
        exit /b 1
    )
) else (
    echo ✅ PyInstaller already installed
)

REM Check pyserial
python -c "import serial" >nul 2>&1
if errorlevel 1 (
    echo 📦 Installing pyserial...
    pip install pyserial
    if errorlevel 1 (
        echo ❌ Failed to install pyserial
        pause
        exit /b 1
    )
) else (
    echo ✅ pyserial already installed
)

echo.
echo ========================================
echo    Building VDU Manager Executable
echo ========================================
echo.

REM Clean previous builds
if exist "build" (
    echo 🧹 Cleaning previous build...
    rmdir /s /q build
)

if exist "dist" (
    echo 🧹 Cleaning previous dist...
    rmdir /s /q dist
)

if exist "VDU_Manager.spec" (
    echo 🧹 Removing old spec file...
    del VDU_Manager.spec
)

echo.
echo 🔨 Building executable with PyInstaller...
echo.

REM Build the executable
pyinstaller --onefile ^
           --windowed ^
           --name="VDU Manager" ^
           --hidden-import=serial ^
           --hidden-import=serial.tools.list_ports ^
           --hidden-import=tkinter ^
           --hidden-import=tkinter.ttk ^
           --hidden-import=tkinter.messagebox ^
           --hidden-import=tkinter.scrolledtext ^
           --hidden-import=tkinter.filedialog ^
           --hidden-import=subprocess ^
           --hidden-import=threading ^
           --hidden-import=datetime ^
           --hidden-import=time ^
           --hidden-import=os ^
           --hidden-import=sys ^
           VDU_Manager.py

if errorlevel 1 (
    echo.
    echo ❌ Build failed!
    echo Check the error messages above.
    pause
    exit /b 1
)

echo.
echo ========================================
echo    Build Complete!
echo ========================================
echo.

REM Check if executable was created
if exist "dist\VDU Manager.exe" (
    echo ✅ Executable created successfully!
    echo 📁 Location: dist\VDU Manager.exe
    echo 📊 Size: 
    for %%A in ("dist\VDU Manager.exe") do echo        %%~zA bytes
    echo.
    echo 🚀 You can now distribute this executable to other computers!
    echo.
    echo 📋 Features included:
    echo    - Firmware upload and build
    echo    - Serial monitoring
    echo    - RTC time setting
    echo    - Log saving
    echo    - No Python installation required
    echo.
    
    REM Ask if user wants to test the executable
    set /p test="Test the executable now? (y/n): "
    if /i "%test%"=="y" (
        echo.
        echo 🧪 Testing executable...
        start "" "dist\VDU Manager.exe"
    )
    
) else (
    echo ❌ Executable not found in dist folder!
    echo Check the build output for errors.
)

echo.
echo Press any key to exit...
pause >nul 