from picamera.array import PiRGBArray # Generates a 3D RGB array
from picamera import PiCamera # Provides a Python interface for the RPi Camera Module
import time # Provides time-related functions
import cv2 # OpenCV library

camera = PiCamera()
camera.resolution = (640,480)
camera.framerate = 32
raw = PiRGBArray(camera, size = (640,480))
time.sleep(0.1)

for frame in camera.capture_continuous(raw, format='bgr',use_video_port=True):
    image = frame.array
    cv2.imshow("Frame", image)
    raw.truncate(0)
    key = cv2.waitKey(1) & 0xFF
    if key == ord('q'):
        break