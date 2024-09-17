import utime
from machine import I2C, Pin

dev = I2C(1, scl=Pin(15), sda=Pin(14))

print(dev.scan())

target_address = 0x48

def readConfig():
    dev.writeto(target_address, bytearray([1]))
    result = dev.readfrom(target_address, 2)
    
    return result[0] << 8 | result[0]

def readValueFrom(channel):
    config = readConfig()
    
    config &= ~(7 << 12) # Clear MUX bits
    config &= ~(7 << 9) # Clear PGA
    
    config |= (7 & (4 + channel)) << 12
    config |= (1 << 15) # Trigger next conversion
    config |= (1 << 9) # gain 4.096V
    
    config = [int(config >> i & 0xff) for i in [8, 0]]
    
    dev.writeto(target_address, bytearray([1] + config))
    
    config = readConfig()
    while (config & 0x8000) == 0:
        config = readConfig()
        
    dev.writeto(target_address, bytearray([0]))
    result = dev.readfrom(target_address, 2)
    
    # Check if result is 0xFFFF and replace it with 0
    result_value = result[0] << 8 | result[0]
    if result_value == 0xFFFF:
        result_value = 0
    
    return result_value
    
val = [0, 0, 0, 0]
module_address = [0x48, 0x49, 0x4A, 0x4B]


#address = 0x4A
#config = readConfig(address)
#print(bin(config))

#address = 0x4B
#config = readConfig(address)
#print(bin(config))

while True:
    val[0] = readValueFrom(0)
    val[1] = readValueFrom(1)
    val[2] = readValueFrom(2)
    val[3] = readValueFrom(3)
    
    print(val)
    #utime.sleep(0.001)
