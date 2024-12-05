
# Thor Controller Software

## Overview
The **Thor Controller Software** is a dual-component system designed to control and monitor the thoracic camera for minimally invasive surgery. The software runs on two devices:
1. **Raspberry Pi Pico**: Handles movement commands and sensor data.
2. **Raspberry Pi 5**: Serves as the control hub, hosting a web-based user interface and managing computer vision processing and UART communication with the Pico.

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
#### Raspberry Pi 5
1. Ensure your Raspberry Pi 5 is running Raspberry Pi OS with Python 3 installed.
2. Enable the camera interface:
   ```bash
   sudo raspi-config
   ```
   Navigate to **Interface Options > Camera** and enable it.

3. Install required system dependencies:
   ```bash
   sudo apt update
   sudo apt install -y python3-opencv python3-picamera2 python3-numpy python3-flask python3-serial
   ```

4. Install additional required libraries:
   ```bash
   sudo apt install -y libcamera-apps libcamera-dev
   ```

5. Ensure the user has the appropriate permissions to use the camera:
   ```bash
   sudo usermod -a -G video $USER
   ```

6. Reboot the Raspberry Pi to apply changes:
   ```bash
   sudo reboot
   ```

#### Raspberry Pi Pico
1. Flash the custom firmware onto the Pico using the Pico SDK or any C development environment.
2. Ensure the Pico is connected to the Raspberry Pi 5 via UART for communication.

---

## Usage

### Raspberry Pi 5
1. Clone this repository to your Raspberry Pi 5:
   ```bash
   git clone <repository_url>
   cd <repository_directory>
   ```

2. Start the Flask server:
   ```bash
   python3 app.py
   ```

3. Open a web browser and navigate to:
   ```bash
   http://<Raspberry-Pi-IP>:5000
   ```
   Replace `<Raspberry-Pi-IP>` with your Raspberry Pi's IP address.

---

## Troubleshooting

### ModuleNotFoundError: No module named 'libcamera'
- Ensure `libcamera` is installed using:
  ```bash
  sudo apt install -y libcamera-apps libcamera-dev
  ```
- Verify the camera interface is enabled:
  ```bash
  sudo raspi-config
  ```
  Navigate to **Interface Options > Camera** and enable it.

### Permission Errors with Camera
- Add the user to the `video` group:
  ```bash
  sudo usermod -a -G video $USER
  ```
- Reboot the Raspberry Pi:
  ```bash
  sudo reboot
  ```

### Dependencies Not Installed
If you encounter dependency installation errors, ensure that you run:
```bash
sudo apt update
sudo apt install -y python3-opencv python3-picamera2 python3-numpy python3-flask python3-serial
```

---

## Notes
- For smoother operation, ensure the Raspberry Pi is connected to a stable network.
- Regularly check for updates to the dependencies using `sudo apt update && sudo apt upgrade`.
