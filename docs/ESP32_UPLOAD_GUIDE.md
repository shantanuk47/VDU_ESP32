# ESP32 Upload Troubleshooting Guide

## Quick Fix: Manual Bootloader Mode

**If upload fails, try this first:**

1. **Hold down the BOOT button** on your ESP32
2. **Press and release the RESET button** (while holding BOOT)
3. **Release the BOOT button**
4. **Try upload again** within 3-5 seconds

## Common Issues & Solutions

### Issue: "Failed to sync with ESP32 bootloader"

**Solution:** Use manual bootloader mode (see above)

### Issue: "Upload timeout"

**Solutions:**
1. Try manual bootloader mode
2. Check USB cable (some cables are charge-only)
3. Try different USB port
4. Close other serial applications

### Issue: "Port not found"

**Solutions:**
1. Check Device Manager for ESP32
2. Install USB drivers (CP210x or CH340)
3. Try different USB cable/port

## Testing Your Setup

### 1. Run Diagnostic Script
```bash
test_esp32.bat
```

### 2. Manual PlatformIO Test
```bash
# Build project
pio run

# Upload with verbose output
pio run -t upload -v

# List devices
pio device list
```

### 3. VDU Manager Test
1. Open VDU Manager
2. Select correct COM port
3. Click "Upload Firmware"

## Hardware Checklist

- [ ] ESP32 connected via USB
- [ ] USB cable supports data (not charge-only)
- [ ] USB drivers installed (CP210x or CH340)
- [ ] ESP32 powered properly
- [ ] No other applications using the serial port

## ESP32 Board Types

### ESP32 DevKit v1
- **BOOT Button**: Usually labeled "IO0" or "BOOT"
- **RESET Button**: Usually labeled "EN" or "RESET"

### ESP32 WROOM
- **BOOT Button**: GPIO0 button
- **RESET Button**: EN button

### ESP32 NodeMCU
- **BOOT Button**: Flash button
- **RESET Button**: Reset button

## Getting Help

If upload continues to fail:

1. **Check the logs** - Look for specific error messages
2. **Try manual CLI** - Use `pio run -t upload -v`
3. **Test hardware** - Try different USB cable/port
4. **Update drivers** - Download latest USB drivers

## Prevention Tips

1. **Always use manual bootloader mode** for reliable uploads
2. **Keep USB drivers updated**
3. **Use quality USB cables**
4. **Avoid static electricity** when handling ESP32 