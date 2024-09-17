import utime
from machine import I2C, Pin

def scan_i2c():
    i2c = I2C(1, scl=Pin(15), sda=Pin(14))
    devices = i2c.scan()
    
    if len(devices) == 0:
        print("No I2C devices found.")
    else:
        print("I2C devices found at addresses:")
        for device in devices:
            print("0x{:02x}".format(device))

# Run the I2C scanner periodically
while True:
    scan_i2c()
    utime.sleep(2)  # Delay between scans (5 seconds in this example)
    print("")
