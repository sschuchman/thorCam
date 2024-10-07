from flask import Flask, Response, render_template_string
from picamera2 import Picamera2
import cv2
import numpy as np
import time
import threading
import serial
import math

app = Flask(__name__)

# Initialize Picamera2
picam2 = Picamera2()
picam2.configure(picam2.create_video_configuration(main={"size": (640, 480), "format": "RGB888"}))
picam2.start()

# Initialize UART connection
uart_port = '/dev/ttyAMA0'
baud_rate = 115200
ser = serial.Serial(uart_port, baud_rate, timeout=1)

# Log data storage
log_data = []

# Global variable to store the distance to the center
distance_to_center = 0

def generate_log_data():
    # This function continuously reads from UART and adds to log_data
    while True:
        if ser.in_waiting > 0:  # Check if data is available
            line = ser.readline().decode('utf-8').strip()  # Read and decode UART line
            log_data.append(line)
            if len(log_data) > 20:  # Limit to the last 20 lines for scrolling effect
                log_data.pop(0)
        time.sleep(0.0001)  # Small delay to avoid overloading the CPU

# Start the log generation in a separate thread
log_thread = threading.Thread(target=generate_log_data)
log_thread.daemon = True
log_thread.start()

# Function to capture frames and perform color-based object tracking
def capture_and_emit_frames():
    global distance_to_center
    while True:
        frame = picam2.capture_array()

        frame_height, frame_width, _ = frame.shape
        center_of_screen = (frame_width // 2, frame_height // 2)

        hsv = cv2.cvtColor(frame, cv2.COLOR_RGB2HSV)

        # Adjusted HSV range for red
        lower_red1 = np.array([0, 100, 100])
        upper_red1 = np.array([10, 255, 255])
        lower_red2 = np.array([160, 100, 100])
        upper_red2 = np.array([180, 255, 255])

        # Create mask for red color
        mask1 = cv2.inRange(hsv, lower_red1, upper_red1)
        mask2 = cv2.inRange(hsv, lower_red2, upper_red2)
        mask = mask1 | mask2

        # Find contours in the mask
        contours, _ = cv2.findContours(mask, cv2.RETR_TREE, cv2.CHAIN_APPROX_SIMPLE)
        
        if contours:
            largest_contour = max(contours, key=cv2.contourArea)

            # Check if the largest contour is above a certain area threshold
            if cv2.contourArea(largest_contour) > 500:
                x, y, w, h = cv2.boundingRect(largest_contour)
                center_of_object = (x + w // 2, y + h // 2)

                # Draw a bounding box and a line to the center of the screen
                cv2.rectangle(frame, (x, y), (x + w, y + h), (0, 255, 255), 2)
                cv2.line(frame, center_of_object, center_of_screen, (255, 0, 0), 2)

                # Calculate the distance to the center
                distance_to_center = int(math.sqrt((center_of_object[0] - center_of_screen[0]) ** 2 +
                                                   (center_of_object[1] - center_of_screen[1]) ** 2))

        # Encode frame as JPEG
        ret, buffer = cv2.imencode('.jpg', frame)
        frame = buffer.tobytes()

        yield (b'--frame\r\n'
               b'Content-Type: image/jpeg\r\n\r\n' + frame + b'\r\n')

@app.route('/')
def index():
    # HTML template with a section for video and a scrolling log window
    return render_template_string('''
    <html>
    <body>
        <h1>Camera Feed with Distance Display</h1>
        <div style="display: flex; gap: 20px;">
            <img src="{{ url_for('video_feed') }}">
            <div style="width: 400px; height: 480px; overflow-y: auto; background-color: #f0f0f0; border: 1px solid #ccc; padding: 10px;">
                <h2>Log Output</h2>
                <div id="log_output"></div>
                <h3>Distance to Center: <span id="distance_display">0</span> px</h3>
            </div>
        </div>
        <script>
            // Function to update log output
            function fetchLogs() {
                fetch("{{ url_for('log_feed') }}")
                    .then(response => response.text())
                    .then(data => {
                        const logOutput = document.getElementById("log_output");
                        logOutput.innerHTML = data.replace(/\\n/g, "<br>"); // Format new lines
                        logOutput.scrollTop = logOutput.scrollHeight;  // Auto-scroll to bottom
                    });
            }
            setInterval(fetchLogs, 1000);  // Fetch logs every second
        </script>
    </body>
    </html>
    ''')

# Video feed route
@app.route('/video_feed')
def video_feed():
    return Response(capture_and_emit_frames(),
                    mimetype='multipart/x-mixed-replace; boundary=frame')

# Log feed route
@app.route('/log_feed')
def log_feed():
    return Response("\n".join(log_data), mimetype='text/plain')

if __name__ == '__main__':
    print("Starting Flask server on port 5000")
    app.run(host='0.0.0.0', port=5000)
