from flask import Flask, Response, render_template, request, jsonify
from picamera2 import Picamera2
import cv2
import numpy as np
import threading
import serial
import math
import time

app = Flask(__name__)

# Initialize Picamera2 with high resolution for live feed
picam2 = Picamera2()
video_config = picam2.create_video_configuration(main={"size": (1920, 1080), "format": "RGB888"})
video_config['controls'] = {"FrameRate": 30}
picam2.configure(video_config)
picam2.start()

# Initialize UART connection
uart_port = '/dev/ttyAMA0'
baud_rate = 115200
ser = serial.Serial(uart_port, baud_rate, timeout=1)

# Log data storage
log_data = []
sensor_values = [0, 0, 0, 0]  # Global variable to store sensor values

# Minimum contour area for valid detection
MIN_CONTOUR_AREA = 20
kernel = np.ones((3, 3), np.uint8)
MASK_RESOLUTION = (320, 240)

# Default values for brightness adjustment and selected color
brightness_increase = 30
selected_color = "green"  # Default color to track

# Define HSV ranges for primary colors and default HSV values for sliders
color_ranges = {
    "red": ([0, 100, 100], [10, 255, 255]),
    "green": ([50, 100, 100], [70, 255, 255]),
    "blue": ([100, 100, 100], [130, 255, 255])
}
lower_hue, upper_hue = color_ranges[selected_color][0][0], color_ranges[selected_color][1][0]  # Default slider values

def generate_log_data():
    """Thread for continuous UART log reading."""
    global sensor_values
    while True:
        if ser.in_waiting > 0:
            line = ser.readline().decode('utf-8').strip()
            log_data.append(line)
            if len(log_data) > 20:
                log_data.pop(0)
            
            # Parse the sensor values from "Load[0,0,0,0]" format
            if line.startswith("Load[") and line.endswith("]"):
                try:
                    values = line[5:-1].split(",")
                    sensor_values = [int(value) for value in values]
                    print(f"Sensor values updated: {sensor_values}")  # Log parsed sensor values
                except ValueError:
                    print(f"Failed to parse sensor values: {line}")

# Start the log generation in a separate thread
log_thread = threading.Thread(target=generate_log_data)
log_thread.daemon = True
log_thread.start()

# Function to generate live video feed with minimal processing and FPS tracking
def generate_video_feed():
    """Capture high-resolution frames for live feed and stream them directly."""
    video_start_time = time.time()
    video_frame_count = 0

    while True:
        frame = picam2.capture_array()
        video_frame_count += 1

        # Calculate FPS and SPF for video feed
        elapsed_time = time.time() - video_start_time
        fps = video_frame_count / elapsed_time if elapsed_time > 0 else 0
        spf = elapsed_time / video_frame_count if video_frame_count > 0 else 0

        # Display FPS and SPF in the top right corner
        cv2.putText(frame, f"FPS: {fps:.2f}", (1500, 50), cv2.FONT_HERSHEY_SIMPLEX, 1.5, (255, 255, 255), 2)
        cv2.putText(frame, f"SPF: {spf:.4f}s", (1500, 100), cv2.FONT_HERSHEY_SIMPLEX, 1.5, (255, 255, 255), 2)

        # Slightly reduce the JPEG quality to increase frame rate
        ret, buffer = cv2.imencode('.jpg', frame, [cv2.IMWRITE_JPEG_QUALITY, 60])
        if ret:
            yield (b'--frame\r\n'
                   b'Content-Type: image/jpeg\r\n\r\n' + buffer.tobytes() + b'\r\n')

# Function to generate mask feed with color detection, overlay, and FPS tracking
def generate_mask_feed():
    """Capture and process low-resolution frames for mask feed with selected color detection."""
    mask_start_time = time.time()
    mask_frame_count = 0
    
    while True:
        frame = picam2.capture_array()
        small_frame = cv2.resize(frame, MASK_RESOLUTION)
        hsv_frame = cv2.cvtColor(small_frame, cv2.COLOR_RGB2HSV)
        hsv_frame[:, :, 2] = np.clip(hsv_frame[:, :, 2] + brightness_increase, 0, 255)

        # Set color range based on selected color and adjusted sliders
        lower_bound = np.array([lower_hue, 100, 100])
        upper_bound = np.array([upper_hue, 255, 255])
        
        mask = cv2.inRange(hsv_frame, lower_bound, upper_bound)
        mask = cv2.morphologyEx(mask, cv2.MORPH_OPEN, kernel, iterations=1)
        mask = cv2.morphologyEx(mask, cv2.MORPH_CLOSE, kernel, iterations=1)

        # Convert mask to BGR to allow color overlay
        mask_colored = cv2.cvtColor(mask, cv2.COLOR_GRAY2BGR)

        # Find contours and overlay bounding box and line in specified colors
        contours, _ = cv2.findContours(mask, cv2.RETR_EXTERNAL, cv2.CHAIN_APPROX_SIMPLE)
        if contours:
            largest_contour = max(contours, key=cv2.contourArea)
            if cv2.contourArea(largest_contour) > MIN_CONTOUR_AREA:
                x, y, w, h = cv2.boundingRect(largest_contour)
                object_center = (x + w // 2, y + h // 2)

                # Center of the screen (for MASK_RESOLUTION)
                screen_center = (MASK_RESOLUTION[0] // 2, MASK_RESOLUTION[1] // 2)

                # Calculate x and y components of distance and total distance
                dx = object_center[0] - screen_center[0]
                dy = -(object_center[1] - screen_center[1])
                distance = int(math.sqrt(dx ** 2 + dy ** 2))

                # Draw yellow bounding box and blue line
                cv2.rectangle(mask_colored, (x, y), (x + w, y + h), (0, 255, 255), 2)  # Yellow bounding box
                cv2.line(mask_colored, screen_center, object_center, (255, 0, 0), 2)  # Blue line to center

                # Display distance and components on mask
                cv2.putText(mask_colored, f"Distance: {distance}", (10, 30), cv2.FONT_HERSHEY_SIMPLEX, 0.6, (255, 255, 255), 2)
                cv2.putText(mask_colored, f"dx: {dx}", (10, 60), cv2.FONT_HERSHEY_SIMPLEX, 0.6, (255, 255, 255), 2)
                cv2.putText(mask_colored, f"dy: {dy}", (10, 90), cv2.FONT_HERSHEY_SIMPLEX, 0.6, (255, 255, 255), 2)

        # Calculate FPS and SPF for mask feed
        mask_frame_count += 1
        elapsed_time = time.time() - mask_start_time
        mask_fps = mask_frame_count / elapsed_time if elapsed_time > 0 else 0
        mask_spf = elapsed_time / mask_frame_count if mask_frame_count > 0 else 0

        # Display FPS and SPF in the top right corner of mask
        cv2.putText(mask_colored, f"FPS: {mask_fps:.2f}", (200, 30), cv2.FONT_HERSHEY_SIMPLEX, 0.6, (255, 255, 255), 2)
        cv2.putText(mask_colored, f"SPF: {mask_spf:.4f}s", (200, 60), cv2.FONT_HERSHEY_SIMPLEX, 0.6, (255, 255, 255), 2)

        ret, buffer = cv2.imencode('.jpg', mask_colored, [cv2.IMWRITE_JPEG_QUALITY, 50])
        if ret:
            yield (b'--frame\r\n'
                   b'Content-Type: image/jpeg\r\n\r\n' + buffer.tobytes() + b'\r\n')

@app.route('/')
def index():
    return render_template('index.html')

# Video feed route
@app.route('/video_feed')
def video_feed():
    return Response(generate_video_feed(), mimetype='multipart/x-mixed-replace; boundary=frame')

# Mask feed route
@app.route('/mask_feed')
def mask_feed():
    return Response(generate_mask_feed(), mimetype='multipart/x-mixed-replace; boundary=frame')

# Log feed route
@app.route('/log_feed')
def log_feed():
    return Response("\n".join(log_data), mimetype='text/plain')

# Endpoint to retrieve the sensor values for bar chart
@app.route('/sensor_values')
def get_sensor_values():
    """Endpoint to retrieve the sensor values."""
    return jsonify(sensor_values=sensor_values)

# Update color range
@app.route('/update_range', methods=['POST'])
def update_range():
    global lower_hue, upper_hue
    data = request.json
    lower_hue = int(data['lower_hue'])
    upper_hue = int(data['upper_hue'])
    return jsonify(success=True)

# Update brightness
@app.route('/update_brightness', methods=['POST'])
def update_brightness():
    global brightness_increase
    data = request.json
    brightness_increase = int(data['brightness'])
    return jsonify(success=True)

# Update color selection and reset hue range
@app.route('/update_color', methods=['POST'])
def update_color():
    global selected_color, lower_hue, upper_hue
    data = request.json
    selected_color = data['color']
    lower_hue, upper_hue = color_ranges[selected_color][0][0], color_ranges[selected_color][1][0]
    return jsonify(success=True, lower_hue=lower_hue, upper_hue=upper_hue)

@app.route('/update_contour_area', methods=['POST'])
def update_contour_area():
    global MIN_CONTOUR_AREA
    data = request.json
    MIN_CONTOUR_AREA = int(data['min_contour_area'])
    return jsonify(success=True)

# New routes for movement controls
@app.route('/move_up', methods=['POST'])
def move_up():
    ser.write(b'MOVE_UP\n')
    return jsonify(success=True, action="MOVE_UP")

@app.route('/move_down', methods=['POST'])
def move_down():
    ser.write(b'MOVE_DOWN\n')
    return jsonify(success=True, action="MOVE_DOWN")

@app.route('/move_left', methods=['POST'])
def move_left():
    ser.write(b'MOVE_LEFT\n')
    return jsonify(success=True, action="MOVE_LEFT")

@app.route('/move_right', methods=['POST'])
def move_right():
    ser.write(b'MOVE_RIGHT\n')
    return jsonify(success=True, action="MOVE_RIGHT")

if __name__ == '__main__':
    print("Starting Flask server on port 5000")
    app.run(host='0.0.0.0', port=5000)