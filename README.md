# esp32-c6-ble-hid-gamepad-remote
ESP32-C6 BLE HID gamepad firmware implementing a 7-button controller with low-power polling, HID-compliant reports, and plug-and-play compatibility.

# ESP32-C6 BLE HID Gamepad

Firmware that turns an ESP32-C6 into a Bluetooth Low Energy (BLE) HID-compliant gamepad.  
The device appears as a standard wireless controller on PCs and mobile devices without requiring custom drivers.

---

## Overview

This project implements a BLE HID gamepad using the ESP32-C6 platform.  
It supports multiple digital buttons, efficient report handling, and power-conscious operation, making it suitable for custom controllers, remotes, and embedded input devices.

The firmware follows standard HID specifications to ensure plug-and-play compatibility with common operating systems.

---

## Features

- BLE HID-compliant gamepad implementation
- 7 digital buttons mapped via HID report descriptor
- X and Y axis support (currently centered)
- Change-based HID report notifications
- Connection-aware processing to reduce power usage
- Onboard LED feedback for button activity
- CPU frequency scaling for balanced performance and efficiency

---

## Hardware Platform

- ESP32-C6 (tested on Seeed Studio XIAO ESP32-C6)
- 7 momentary push buttons
- Onboard LED (used for activity indication)

---

## How It Works

- ESP32-C6 advertises as a BLE HID gamepad
- Button states are packed into a HID input report
- Reports are sent only when input state changes
- Host device interprets input as a standard game controller

---

## HID Report Structure

- Byte 0: Button bitmask (7 buttons + padding)
- Byte 1: X axis (signed 8-bit)
- Byte 2: Y axis (signed 8-bit)

This structure follows the Generic Desktop / Gamepad HID usage.

---

## Power Behavior

- CPU frequency dynamically scales between 80 MHz and 160 MHz
- BLE reports are sent only when connected
- Reduced polling when no host is connected

---

## Compatibility

Tested and compatible with:
- Windows
- Linux
- Android
- macOS (BLE HID support required)

No additional drivers are required.

---

## Applications

- Custom BLE game controllers
- Wireless HID remotes
- Assistive input devices
- DIY embedded input peripherals
- Embedded systems and IoT portfolios

---

## Author

MD Hashim  
Embedded Systems and IoT Projects

---

## License

MIT License
