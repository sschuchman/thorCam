# Thoracic Camera

# Contributors
Sean Schuchman: January 2024 - Present

Jakob Schoeberle: August 2024 - Present

Colton Wall: August 2024 - Present

# Hardware
- Raspberry Pi 5
    - Handle camera module video feed
    - Computer vision to determine location of target
    - Send input to Pico Controller to handle actuation of camera in desired direction

- Pico Controller
    - Takes in input of direction to drive the camera actuation
    - Takes in sensor data from tension modules to maintain proper control wire tension
    - Drives stepper motors for tension wire control

# Pico Controller
The Pico Controller is designed to serve as the abstraction layer between the computer vision element and the camera actuation control. This is intended to simplify the project by containerizing the physical movement and control of the camera into a simple to interface.

The Pico Controller is configured to handle camera actuation by control of the stepper motor wires, as well as receive data from the tension modules to adjust the control wires to maintain proper balance.

The Pico Controller can be controlled manually or via remote software driver with simple controls to actuate the camera in any combination of x-plane or y-plane directions.

# Setup

## Pi Pico Setup

## Raspberry Pi Setup
