import machine
import utime
#import adafruit_ads1x15.ads1115 as ADS


ADS1115_ADDRESS = 0x48
CONFIG_REG = 0x01
CONVERSION_REG = 0x00

config = b'\xC3\83'
        
i2c = machine.I2C(0, scl=machine.Pin(1), sda=machine.Pin(0))

i2c.writeto(ADS1115_ADDRESS, config)

def read_adc():
    i2c.writeto(ADS1115_ADDRESS, b'\0x01')
    
    return int.from_bytes(i2c.readfrom(ADS1115_ADDRESS, 2), 'big', signed=True)

print("ADC Module Startup...")
while True:
    adc_value = read_adc()
    #print("ADC Value: ", adc_value)
    utime.sleep(1)
