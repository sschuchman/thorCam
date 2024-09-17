from time import sleep
import motor
import tkinter as tk
import tkinter.font as font
import subprocess
def openCameraCommand():
    if openCamera['text'] == 'Open Camera':
        openCamera['text'] = 'Close Camera'
        process = subprocess.Popen(["python","cam.py"])
    else:
        openCamera['text'] = 'Open Camera'
def tightCommand():
    if(tight['text'] == 'Tighten'):
        tight['text']='Loosen'
    else:
        tight['text']='Tighten'
def Right():
    # Turn motor0 and motor3 together
    if(tight['text'] == 'Tighten'):
        # Clockwise
        delay = float(speed.get())*(.999/-200.0)+1.00
        motor0.mvCW()
        motor3.mvCW()
        sleep(delay)
    else:
        # Counter-clockwise
        delay = float(speed.get())*(.999/-200.0)+1.00
        motor0.mvCCW()
        motor3.mvCCW()
        sleep(delay)
def Left():
    # Turn motor1 and motor2 together
    if(tight['text'] == 'Tighten'):
        # Clockwise
        delay = float(speed.get())*(.999/-200.0)+1.00
        motor1.mvCW()
        motor2.mvCW()
        sleep(delay)
    else:
        # Counter-clockwise
        delay = float(speed.get())*(.999/-200.0)+1.00
        motor1.mvCCW()
        motor2.mvCCW()
        sleep(delay)
def Up():
    # Turn motor0 and motor1 together
    if(tight['text'] == 'Tighten'):
        # Clockwise
        delay = float(speed.get())*(.999/-200.0)+1.00
        motor0.mvCW()
        motor1.mvCW()
        sleep(delay)
    else:
        # Counter-clockwise
        delay = float(speed.get())*(.999/-200.0)+1.00
        motor0.mvCCW()
        motor1.mvCCW()
        sleep(delay)
def Down():
    # Turn motor2 and motor3 together
    if(tight['text'] == 'Tighten'):
        # Clockwise
        delay = float(speed.get())*(.999/-200.0)+1.00
        motor2.mvCW()
        motor3.mvCW()
        sleep(delay)
    else:
        # Counter-clockwise
        delay = float(speed.get())*(.999/-200.0)+1.00
        motor2.mvCCW()
        motor3.mvCCW()
        sleep(delay)
def release(self):
    motor0.ResetPins()
    motor1.ResetPins()
    motor2.ResetPins()
    motor3.ResetPins()
# Initialization of motors
motor0 = motor.Motor(2,3,4,15)
motor1 = motor.Motor(17,18,27,22)
motor2 = motor.Motor(23,24,10,9)
motor3 = motor.Motor(25,11,8,7)
# Create main window
window = tk.Tk()
window.title("Motor Test GUI")
# Create frames
frameList = []
for i in range(0,3):
    for j in range(0,3):
        frame = tk.Frame(master = window,padx = 20, pady=20,width = 200, height = 200, bg = 'white',borderwidth =4,relief = tk.RAISED)
        frameList.append(frame)
        frame.grid(row = i, column = j, pady=2, padx=2)
# Create buttons for arrows
upButton = tk.Button(master = frameList[1],bg = 'white',text='↑',font=font.Font(size='45'),command=Up,repeatdelay=1,repeatinterval=1)
upButton.pack(fill=tk.BOTH)
upButton.bind("<ButtonRelease>",release)
leftButton = tk.Button(master = frameList[3],bg = 'white',text='←',font=font.Font(size='45'),command=Left,repeatdelay=1,repeatinterval=1)
leftButton.pack(fill=tk.BOTH)
leftButton.bind("<ButtonRelease>",release)
rightButton = tk.Button(master = frameList[5],bg = 'white',text='→',font=font.Font(size='45'),command=Right,repeatdelay=1,repeatinterval=1)
rightButton.pack(fill=tk.BOTH)
rightButton.bind("<ButtonRelease>",release)
downButton = tk.Button(master = frameList[7],bg = 'white',text='↓',font=font.Font(size='45'),command=Down,repeatdelay=1,repeatinterval=1)
downButton.pack(fill=tk.BOTH)
downButton.bind("<ButtonRelease>",release)
# Create main speed slider and tighten/loosen button
speed = tk.Scale(master = frameList[4], from_=0, to=199,orient=tk.HORIZONTAL)
tight = tk.Button(master = frameList[4],text='Tighten',command=tightCommand)
openCamera = tk.Button(master = frameList[4], text = 'Open Camera', command = openCameraCommand)
speed.pack(fill=tk.BOTH)
tight.pack(fill=tk.BOTH)
openCamera.pack(fill=tk.BOTH)

window.mainloop()