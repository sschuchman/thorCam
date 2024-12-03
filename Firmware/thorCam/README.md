# Thor Controller Firmware

## Overview
The **Thor Controller Firmware** is designed to run on the Raspberry Pi Pico, facilitating movement control and sensor monitoring for the thoracic camera system. The firmware communicates with the Raspberry Pi 5 over UART, processing movement commands and controlling stepper motors while continuously monitoring sensor data.

The firmware leverages a modular structure, with functionalities like motor control, sensor monitoring, and UART communication implemented in dedicated source and header files.

---

## Features
- **UART Communication**:
  - Receives movement commands (`Move[x, y]`) from the Raspberry Pi 5.
  - Sends real-time sensor data (`Load[0, 0, 0, 0]`) back to the Raspberry Pi 5.
- **Sensor Monitoring**:
  - Reads tension sensor values using ADS1115 ADCs connected over I2C.
  - Monitors sensor thresholds to maintain balance and ensure safe motor operation.
- **Motor Control**:
  - Controls four stepper motors via L293D motor drivers.
  - Supports precise movement in response to parsed UART commands.
- **Multicore Processing**:
  - Core 0 handles UART communication and movement control.
  - Core 1 continuously reads sensor values for real-time feedback.

---

## Installation

### Prerequisites
- **Development Environment**:
  - Install Visual Studio Code with the Raspberry Pi Pico extension.
  - Configure the extension to build and run Pico projects.
- **Debugging Tools**:
  - Use the Raspberry Pi Pico Debug Probe to flash firmware and debug.
- **Hardware Setup**:
  - Connect:
    - ADS1115 ADC modules to the I2C pins (SDA: GPIO4, SCL: GPIO5).
    - Stepper motors to L293D drivers and appropriate GPIO pins.
    - UART (GPIO0, GPIO1) for communication with the Raspberry Pi 5.

### Build and Flash
1. Clone the repository and open the `thorCam` folder in Visual Studio Code.
2. Build the firmware using the Pico extension's build tools.
3. Flash the firmware to the Raspberry Pi Pico using the Pico Debug Probe.

---

## Key Functionalities

### 1. **Main File**
- `main.c`: Initializes all components and starts the main execution loop.
  - Sets up UART, I2C, and GPIO configurations.
  - Spawns tasks for:
    - Reading sensors on Core 1.
    - Handling UART commands and movement control on Core 0.

### 2. **Functional Highlights**
- **Motor Control**:
  - `l293d.c` and `l293d.h` provide functions to initialize and control stepper motors.
  - Implements precise stepping logic for both forward and backward movement.
- **Sensor Monitoring**:
  - Interfaces with ADS1115 modules to read differential sensor values.
  - Calibrates sensors and monitors tension thresholds.
- **Movement**:
  - Maps movement commands to motor actions.
  - Ensures balanced tension during movement by coordinating multiple motors.
- **UART Communication**:
  - Parses incoming UART commands such as `Move[x, y]`.
  - Sends sensor data logs back to the Raspberry Pi 5.
- **ADC Handling**:
  - Provides an interface for configuring and reading from ADS1115 ADC modules.
  - Supports differential and single-ended readings.

---

## Usage
1. Flash the firmware to the Raspberry Pi Pico using the Debug Probe.
2. Connect the hardware as specified in the prerequisites.
3. Use the Flask web interface running on the Raspberry Pi 5 to send movement commands and monitor sensor feedback.

---

## Troubleshooting
- **No UART Communication**:
  - Verify the correct UART pins (GPIO0, GPIO1) are connected to the Raspberry Pi 5.
  - Check the baud rate matches between the devices (default: `115200`).
- **Motor Stalling or Overrun**:
  - Ensure motor driver connections are secure.
  - Verify tension limits in the sensor monitoring code and adjust as necessary.
- **Incorrect Sensor Readings**:
  - Confirm I2C connections (SDA: GPIO4, SCL: GPIO5).
  - Calibrate the ADS1115 modules using the `ads1115_calibrate()` function.
