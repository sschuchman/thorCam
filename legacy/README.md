# ThorCam
## URCA

# Contributors
Sean Schuchman: January 2024 - Present

# Setup

# Push to Remote
*** git push [URL] will not work:
```
git init
git add .
git commit -m "<insert comment here>"
git push --set-upstream origin --all
```

# Project Hierarchy
- Raspberry Pi 4
    - Handle camera module video feed
    - Computer vision to determine location of target
    - Send input to Pico Controller to handle actuation of camera in desired direction

- Pico Controller
    - Takes in input of direction to drive the camera actuation
    - Takes in sensor data from tension modules to maintain proper control wire tension
    - Drives steppor motors for tension wire control

- Tension Module
    - Reads control wire tension via pressure sensor
    - Analog signal fed through Op-Amp buffer to ADC module
    - ACD module connected to I2C bus
    - Four tension modules for each control wire are connected along Pico Controller I2C bus by the ACD module I2C interface
    - Tension module provides analog data on pressure sensor, as well as target tension by means of analog voltage

# Pico Controller
The Pico Controller is designed to serve as the abstration layer between the computer vision element and the camera actuation control. This is intended to simplify the project by containerizing the physical movement and control of the camera into a simple to interface.

The Pico Controller is configured to handle camera actuation by control of the stepper motor wires, as well as receive data from the tension modules to adjust the control wires to maintain proper balance.

The Pico Controller can be controlled manually or via remote software driver with simple controls to actuate the camera in any combination of x-plane or y-plane directions.

Project files can be found under the project directory:
/dev/pico_controller

## Hardware
- Raspberry Pi Pico
    - RP2040 microcontroller chip
    - Dual-core ARM Cortex M0+ processor, clock up to 133 MHz
    - 264kB SRAM, 2MB on-board flash memory

## Dev Environment
- Raspberry Pi 4
    - Thonny IDE
    - MircoPython SDK for Pi Pico

## Raspberry Pi Pico Setup
Follow steps in YouTube tutorial for setup:
https://www.youtube.com/watch?v=Zy64kZEM_bg

1. Connect Pi Pico to Raspberry Pi 4 (or alternative version) via USB. Be sure to hold down reset button on Pi Pico while connecting to enable bootloader mode.
2. Access Pi Pico as a directory in file explorer.
3. Copy the MicroPython library 'RPI_PICO-20240105-v1.22.1.uf2' to the Pi Pico. MicroPython UF2 library file can be found in project directory: /dev/pico_controller.
4. Pi Pico should be ready to target as available device in Thonny IDE.

More information can be found at Raspberry Pi Pico Docs:
https://www.raspberrypi.com/documentation/microcontrollers/micropython.html

- Download the latest UF2 file from this page. Found under "Download the correct MicroPython UF2 file for your board:" and select the appropriate device.
