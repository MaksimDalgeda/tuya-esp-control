# Tuya ESP Control

An OpenWRT-based solution that enables controlling ESP devices from Tuya Cloud through UBUS communication.

## Overview

The project consists of two separate applications:

1. **ESP Controller**
   - Detects active ESP devices.
   - Controls ESP GPIO pins.
   - Reads sensor data from ESP devices.
   - Exposes functionality through UBUS.

2. **Tuya Client**
   - Connects to Tuya Cloud.
   - Receives actions from Tuya platform.
   - Sends UBUS requests to ESP Controller.
   - Returns operation results back to Tuya Cloud.

## Features

- List active ESP devices
- Control ESP GPIO pins (ON/OFF)
- Read sensor data
- UBUS-based inter-process communication
- Error and success response handling

## Architecture

```text
Tuya Cloud
     |
     v
Tuya Client
     |
    UBUS
     |
     v
ESP Controller
     |
Serial/UART
     |
     v
ESP Devices