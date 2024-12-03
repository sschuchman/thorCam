# Thor Controller Software

## Overview
The **Thor Controller Software** is a dual-component system designed to control and monitor the thoracic camera for minimally invasive surgery. The software runs on two devices:
1. A **Raspberry Pi Pico**, which handles movement commands and sensor data.
2. A **Raspberry Pi 5**, which serves as the control hub, hosting a web-based user interface and managing computer vision processing and UART communication with the Pico.

This system provides a seamless interface for real-time camera movement, video visualization, and automatic tracking of surgical tools.

---

## Features
### Raspberry Pi 5
- **Web Application**:
  - Built with Flask, serving as the main user interface.
  - Displays:
    - High-resolution live camera feed from the Pi Camera V3 Wide.
    - Mask feed showing object tracking and offset visualization.
    - Real-time sensor data in a bar chart.
    - Live UART log output from the Raspberry Pi Pico.
  - Supports toggling between **manual** and **automatic** control modes.
  - Interactive sliders to adjust brightness, tracking thresholds, and color detection parameters.
- **Computer Vision**:
  - Uses OpenCV to detect and track objects based on color.
  - Computes positional offsets `(dx, dy)` of the target relative to the center of the screen.
  - In automatic mode, calculates and sends movement commands to the Raspberry Pi Pico for continuous adjustments.
- **UART Communication**:
  - Sends movement commands (`Move[x, y]`) to the Raspberry Pi Pico.
  - Receives live sensor data and logs from the Pico for display.

### Raspberry Pi Pico
- Processes UART commands received from the Raspberry Pi 5.
- Adjusts camera position based on movement commands.
- Reads and processes tension sensor data using ADC inputs.
- Manages motor control via L293D motor drivers.

---

## Installation

### Prerequisites
- **Raspberry Pi 5**:
  - Python 3
  - Required libraries:
    - Flask
    - OpenCV (`cv2`)
    - numpy
    - picamera2
    - serial
  - Install dependencies:
    ```bash
    pip install flask opencv-python numpy picamera2 pyserial
    ```

- **Raspberry Pi Pico**:
  - Flash the custom firmware onto the Pico using a C development environment like the Pico SDK.

---

## Usage

### Raspberry Pi 5
1. Start the Flask server:
  ```bash
    python app.py
  ```
2. Open a web browser and navigate to http://<Raspberry-Pi-IP>:5000
