import utime
from machine import I2C, Pin

dev = I2C(1, scl=Pin(15), sda=Pin(14))

print(dev.scan())

def readConfig(address):
    dev.writeto(address, bytearray([1]))
    result = dev.readfrom(address, 2)
    
    return result[0] << 8 | result[0]

def readValueFrom(address, channel):
    config = readConfig(address)
    
    config &= ~(7 << 12) # Clear MUX bits
    config &= ~(7 << 9) # Clear PGA
    
    config |= (7 & (4 + channel)) << 12
    config |= (1 << 15) # Trigger next conversion
    config |= (1 << 9) # gain 4.096V
    
    config = [int(config >> i & 0xff) for i in [8, 0]]
    
    dev.writeto(address, bytearray([1] + config))
    
    config = readConfig(address)
    while (config & 0x8000) == 0:
        config = readConfig(address)
        
    dev.writeto(address, bytearray([0]))
    result = dev.readfrom(address, 2)
    
    # Check if result is 0xFFFF and replace it with 0
    result_value = result[0] << 8 | result[0]
    if result_value == 0xFFFF:
        result_value = 0
    
    return result_value
    
module_addresses = [0x48, 0x49, 0x4A, 0x4B]
channels = [0, 1, 2, 3]
num_addresses = len(module_addresses)
num_channels = len(channels)

while True:
    for address in module_addresses:
        readings = []
        for channel in channels:
            val = readValueFrom(address, channel)
            readings.append(val)
        print("Address:", hex(address), "Readings:", readings)
        utime.sleep(1)

