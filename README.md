# Thoracic Camera

## Overview
The **Thoracic Camera** project is a collaborative effort to design a dynamic thoracic camera system capable of precision actuation and real-time feedback for use in minimally invasive surgeries. By combining a Raspberry Pi 5 for advanced processing and a Raspberry Pi Pico for motor control and sensor feedback, the system delivers a robust solution for camera tracking and actuation in the chest cavity.

---

## Contributors
- **Sean Schuchman**: January 2024 - December 2024  
- **Jakob Schoeberle**: August 2024 - December 2024  
- **Colton Wall**: August 2024 - December 2024  

---

## Hardware

### Raspberry Pi 5
- **Role**: Central processing hub for the system.  
- **Responsibilities**:
  - Handles video feed from the Pi Camera Module V3.
  - Performs computer vision tasks to detect and track a target object.
  - Sends actuation commands to the Pico Controller via UART.

### Pico Controller (Raspberry Pi Pico)
- **Role**: Camera actuation controller.  
- **Responsibilities**:
  - Interprets movement commands from the Raspberry Pi 5 and drives stepper motors.
  - Monitors sensor feedback from tension modules to ensure proper wire tension.
  - Handles precise actuation of the thoracic camera in response to commands.

---

## Pico Controller
The **Pico Controller** acts as an abstraction layer between the computer vision system on the Raspberry Pi 5 and the physical camera actuation system. It simplifies control by encapsulating motor and tension management, providing an easy-to-interface control system.  

### Features:
- **Camera Actuation**:  
  Controls stepper motors to manipulate tension wires, enabling the camera to move along the x-plane and y-plane directions.  
- **Tension Monitoring**:  
  Uses sensor feedback from load cells to dynamically adjust wires and maintain system balance.  
- **Control Modes**:  
  - **Manual Mode**: Direct input to move the camera.  
  - **Automatic Mode**: Actuation based on computer vision commands from the Raspberry Pi 5.  

---

## Directory Structure
The project is divided into several directories to organize hardware, software, and firmware resources.

- `/Software`: Contains software running on the Raspberry Pi 5, including computer vision and the Flask web application for control.  
- `/Firmware`: Firmware for the Raspberry Pi Pico to manage motor control and tension monitoring.  
- `/Hardware`: Schematic diagrams, PCB designs, and other hardware resources.  
- `/Datasheets`: Documentation for components such as stepper motors, motor drivers, and sensors.  
- `/3D_Models`: 3D printable designs for physical components.  
- `/legacy`: Archive of previous iterations or deprecated designs.

---

## Setup

### Raspberry Pi Pico Setup
1. **Development Environment**:
   - Use Visual Studio Code with the Raspberry Pi Pico extension to build and flash the firmware.
   - Ensure the Raspberry Pi Pico Debug Probe is connected for flashing and debugging.  

2. **Firmware Installation**:
   - Clone the repository and navigate to the `/Firmware` directory.
   - Open the project in Visual Studio Code and use the Pico extension to compile and flash the firmware.  

3. **Hardware Connections**:
   - Connect stepper motors via L293D drivers to the GPIO pins.
   - Connect ADS1115 ADC modules to GPIO4 (SDA) and GPIO5 (SCL).
   - Ensure UART pins (GPIO0 and GPIO1) are connected to the Raspberry Pi 5 for communication.

### Raspberry Pi 5 Setup
1. **Install Required Software**:
   - Ensure Python is installed with the required libraries (`Flask`, `OpenCV`, etc.).
   - Navigate to the `/Software` directory and install dependencies using:
     ```bash
     pip install -r requirements.txt
     ```

2. **Start the Web Application**:
   - Run the `app.py` script to start the Flask web server:
     ```bash
     python app.py
     ```
   - Access the control interface via a web browser on the local network.

3. **Hardware Connections**:
   - Connect the Pi Camera Module V3 to the CSI port.
   - Ensure UART is configured on `/dev/ttyAMA0` for communication with the Pico Controller.

---

## Usage
1. **Manual Control**:
   - Use the web interface to manually send movement commands to the Pico Controller.
   - Adjust camera settings and monitor sensor feedback through the interface.

2. **Automatic Control**:
   - Enable automatic mode via the web interface to allow the system to track a selected object.
   - The Raspberry Pi 5 processes the video feed to calculate the offset and sends movement commands to the Pico Controller.

3. **Real-Time Feedback**:
   - Monitor sensor values and system logs in the web interface.
   - View live video and mask feeds to verify object tracking.
