from flask import Flask, Response, render_template_string, request, jsonify
from picamera2 import Picamera2
import cv2
import numpy as np
import threading
import serial
import math

app = Flask(__name__)

# Initialize Picamera2 with high resolution for live feed
picam2 = Picamera2()
picam2.configure(picam2.create_video_configuration(main={"size": (640, 480), "format": "RGB888"}))
picam2.start()

# Initialize UART connection
uart_port = '/dev/ttyAMA0'
baud_rate = 115200
ser = serial.Serial(uart_port, baud_rate, timeout=1)

# Log data storage
log_data = []

# Minimum contour area for valid detection
MIN_CONTOUR_AREA = 500
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
    while True:
        if ser.in_waiting > 0:
            line = ser.readline().decode('utf-8').strip()
            log_data.append(line)
            if len(log_data) > 20:
                log_data.pop(0)

# Start the log generation in a separate thread
log_thread = threading.Thread(target=generate_log_data)
log_thread.daemon = True
log_thread.start()

# Function to generate live video feed with minimal processing
def generate_video_feed():
    """Capture high-resolution frames for live feed and stream them directly."""
    while True:
        frame = picam2.capture_array()
        ret, buffer = cv2.imencode('.jpg', frame, [cv2.IMWRITE_JPEG_QUALITY, 70])
        if ret:
            yield (b'--frame\r\n'
                   b'Content-Type: image/jpeg\r\n\r\n' + buffer.tobytes() + b'\r\n')

# Function to generate mask feed with color detection and overlay at lower frame rate
def generate_mask_feed():
    """Capture and process low-resolution frames for mask feed with selected color detection."""
    global lower_hue, upper_hue, brightness_increase, selected_color
    
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

                # Draw yellow bounding box and blue line
                cv2.rectangle(mask_colored, (x, y), (x + w, y + h), (0, 255, 255), 2)  # Yellow bounding box
                cv2.line(mask_colored, screen_center, object_center, (255, 0, 0), 2)  # Blue line to center

                # Calculate distance between the centers
                distance = int(math.sqrt((object_center[0] - screen_center[0]) ** 2 +
                                         (object_center[1] - screen_center[1]) ** 2))

                # Display distance on mask
                cv2.putText(mask_colored, f"Distance: {distance}", (10, 30), cv2.FONT_HERSHEY_SIMPLEX, 0.7, (255, 255, 255), 2)

        ret, buffer = cv2.imencode('.jpg', mask_colored, [cv2.IMWRITE_JPEG_QUALITY, 50])
        if ret:
            yield (b'--frame\r\n'
                   b'Content-Type: image/jpeg\r\n\r\n' + buffer.tobytes() + b'\r\n')

@app.route('/')
def index():
    return render_template_string('''
    <html>
        <body>
            <h1>Camera Feed with Log Output</h1>
            <div style="display: flex; gap: 20px;">
                <div>
                    <h2>Live Camera Feed</h2>
                    <img src="{{ url_for('video_feed') }}" width="640" height="480">
                </div>
                <div>
                    <h2>Mask Feed</h2>
                    <img src="{{ url_for('mask_feed') }}" width="320" height="240">
                </div>
                <div style="width: 400px; height: 480px; overflow-y: auto; background-color: #f0f0f0; border: 1px solid #ccc; padding: 10px;">
                    <h2>Log Output</h2>
                    <div id="log_output"></div>
                </div>
            </div>
            <div>
                <h3>Select Color to Track</h3>
                <select id="color_selector" onchange="updateColor()">
                    <option value="red">Red</option>
                    <option value="green" selected>Green</option>
                    <option value="blue">Blue</option>
                </select>
                <h3>Adjust Color Range</h3>
                <label>Lower Hue: 
                    <input type="range" id="lower_hue" min="0" max="180" oninput="updateValue('lower_hue')">
                    <span id="lower_hue_value">50</span>
                </label>
                <br>
                <label>Upper Hue: 
                    <input type="range" id="upper_hue" min="0" max="180" oninput="updateValue('upper_hue')">
                    <span id="upper_hue_value">70</span>
                </label>
                <h3>Brightness Control</h3>
                <label>Brightness: 
                    <input type="range" id="brightness" min="0" max="100" value="30" oninput="updateValue('brightness')">
                    <span id="brightness_value">30</span>
                </label>
            </div>
            <script>
                function fetchLogs() {
                    fetch("{{ url_for('log_feed') }}")
                        .then(response => response.text())
                        .then(data => {
                            const logOutput = document.getElementById("log_output");
                            logOutput.innerHTML = data.replace(/\\n/g, "<br>");
                            logOutput.scrollTop = logOutput.scrollHeight;
                        });
                }
                setInterval(fetchLogs, 1000);

                function updateValue(id) {
                    const slider = document.getElementById(id);
                    const display = document.getElementById(id + '_value');
                    display.textContent = slider.value;

                    if (id === "lower_hue" || id === "upper_hue") {
                        updateRange();
                    } else if (id === "brightness") {
                        updateBrightness();
                    }
                }

                function updateRange() {
                    const lowerHue = document.getElementById("lower_hue").value;
                    const upperHue = document.getElementById("upper_hue").value;
                    fetch("/update_range", {
                        method: "POST",
                        headers: { "Content-Type": "application/json" },
                        body: JSON.stringify({ lower_hue: lowerHue, upper_hue: upperHue })
                    });
                }

                function updateBrightness() {
                    const brightness = document.getElementById("brightness").value;
                    fetch("/update_brightness", {
                        method: "POST",
                        headers: { "Content-Type": "application/json" },
                        body: JSON.stringify({ brightness: brightness })
                    });
                }

                function updateColor() {
                    const color = document.getElementById("color_selector").value;
                    fetch("/update_color", {
                        method: "POST",
                        headers: { "Content-Type": "application/json" },
                        body: JSON.stringify({ color: color })
                    }).then(response => response.json())
                      .then(data => {
                        document.getElementById("lower_hue").value = data.lower_hue;
                        document.getElementById("upper_hue").value = data.upper_hue;
                        document.getElementById("lower_hue_value").textContent = data.lower_hue;
                        document.getElementById("upper_hue_value").textContent = data.upper_hue;
                    });
                }
            </script>
        </body>
    </html>
    ''')

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

if __name__ == '__main__':
    print("Starting Flask server on port 5000")
    app.run(host='0.0.0.0', port=5000)
