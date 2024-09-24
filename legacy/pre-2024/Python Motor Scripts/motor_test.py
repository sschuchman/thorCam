from time import sleep
import motor
import tkinter as tk

fromMotorFile = open('motorfile.txt','r')
fromMotorFile.readlines()

def TLCW():
    delay = float(TLCWScale.get())*(.999/-200.0)+1.00
    motor0.mvCW()
    sleep(delay)
def TLCCW():
    delay = float(TLCWScale.get())*(.999/-200.0)+1.00
    motor0.mvCCW()
    sleep(delay)
def TRCW():
    delay = float(TRCWScale.get())*(.999/-200.0)+1.00
    motor1.mvCW()
    sleep(delay)
def TRCCW():
    delay = float(TRCWScale.get())*(.999/-200.0)+1.00
    motor1.mvCCW()
    sleep(delay)
def BLCW():
    delay = float(BLCWScale.get())*(.999/-200.0)+1.00
    motor2.mvCW()
    sleep(delay)
def BLCCW():
    delay = float(BLCWScale.get())*(.999/-200.0)+1.00
    motor2.mvCCW()
    sleep(delay)
def BRCW():
    delay = float(BRCWScale.get())*(.999/-200.0)+1.00
    motor3.mvCW()
    sleep(delay)
def BRCCW():
    delay = float(BRCWScale.get())*(.999/-200.0)+1.00
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
frameTL = tk.LabelFrame(master = window,text = 'Motor 0',padx = 20, pady=20,width = 400, height = 400, bg = 'white',borderwidth =4,relief = tk.RAISED)
frameTR = tk.LabelFrame(master = window,text = 'Motor 1',padx = 20, pady=20,width = 400, height = 400, bg = 'white',borderwidth =4,relief = tk.RAISED)
frameBL = tk.LabelFrame(master = window,text = 'Motor 2',padx = 20, pady=20,width = 400, height = 400, bg = 'white',borderwidth =4,relief = tk.RAISED)
frameBR = tk.LabelFrame(master = window,text = 'Motor 3',padx = 20, pady=20,width = 400, height = 400, bg = 'white',borderwidth =4,relief = tk.RAISED)
# Pack frames in grid format
frameTL.grid(row = 0, column = 0, pady = 2, padx = 2)
frameTR.grid(row = 0, column = 1, pady = 2, padx = 2)
frameBL.grid(row = 1, column = 0, pady = 2, padx = 2)
frameBR.grid(row = 1, column = 1, pady = 2, padx = 2)
# Create buttons for clockwise and counter-clockwise movement along with slider
buttonFrame1 = tk.Frame(master = frameTL)
buttonFrame1.pack()
TLCWButton = tk.Button(master = buttonFrame1,text = 'Clockwise',repeatdelay=1,repeatinterval=1,command = TLCW)
TLCWButton.grid(row=0, column = 0)
TLCWButton.bind("<ButtonRelease>",release)
TLCWScale = tk.Scale(master = frameTL, from_=0, to=199,orient=tk.HORIZONTAL)
TLCWScale.pack(side=tk.BOTTOM,fill=tk.BOTH)
TLCCWButton = tk.Button(master = buttonFrame1,text = 'Counter-Clockwise',repeatdelay=1,repeatinterval=1,command = TLCCW)
TLCCWButton.grid(row=0,column = 1)
TLCCWButton.bind("<ButtonRelease>",release)

buttonFrame2 = tk.Frame(master = frameTR)
buttonFrame2.pack()
TRCWButton = tk.Button(master = buttonFrame2,text = 'Clockwise',repeatdelay=1,repeatinterval=1,command = TRCW)
TRCWButton.grid(row=0, column = 0)
TRCWButton.bind("<ButtonRelease>",release)
TRCWScale = tk.Scale(master = frameTR, from_=0, to=199,orient=tk.HORIZONTAL)
TRCWScale.pack(side=tk.BOTTOM,fill=tk.BOTH)
TRCCWButton = tk.Button(master = buttonFrame2,text = 'Counter-Clockwise',repeatdelay=1,repeatinterval=1,command = TRCCW)
TRCCWButton.grid(row=0,column = 1)
TRCCWButton.bind("<ButtonRelease>",release)

buttonFrame3 = tk.Frame(master = frameBL)
buttonFrame3.pack()
BLCWButton = tk.Button(master = buttonFrame3,text = 'Clockwise',repeatdelay=1,repeatinterval=1,command = BLCW)
BLCWButton.grid(row=0, column = 0)
BLCWButton.bind("<ButtonRelease>",release)
BLCWScale = tk.Scale(master = frameBL, from_=0, to=199,orient=tk.HORIZONTAL)
BLCWScale.pack(side=tk.BOTTOM,fill=tk.BOTH)
BLCCWButton = tk.Button(master = buttonFrame3,text = 'Counter-Clockwise',repeatdelay=1,repeatinterval=1,command = BLCCW)
BLCCWButton.grid(row=0,column = 1)
BLCCWButton.bind("<ButtonRelease>",release)

buttonFrame4 = tk.Frame(master = frameBR)
buttonFrame4.pack()
BRCWButton = tk.Button(master = buttonFrame4,text = 'Clockwise',repeatdelay=1,repeatinterval=1,command = BRCW)
BRCWButton.grid(row=0, column = 0)
BRCWButton.bind("<ButtonRelease>",release)
BRCWScale = tk.Scale(master = frameBR, from_=0, to=199,orient=tk.HORIZONTAL)
BRCWScale.pack(side=tk.BOTTOM,fill=tk.BOTH)
BRCCWButton = tk.Button(master = buttonFrame4,text = 'Counter-Clockwise',repeatdelay=1,repeatinterval=1,command = BRCCW)
BRCCWButton.grid(row=0,column = 1)
BRCCWButton.bind("<ButtonRelease>",release)



window.mainloop()
