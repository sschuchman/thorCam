#ifndef ADS1115_H
#define ADS1115_H

#include "hardware/i2c.h"

// ADS1115 I2C Address
#define ADS1115_ADDRESS_GND 0x48
#define ADS1115_ADDRESS_VDD 0x49
#define ADS1115_ADDRESS_SDA 0x4A
#define ADS1115_ADDRESS_SCL 0x4B

// ADS1115 Register Pointers
#define ADS1115_POINTER_CONVERSION 0x00
#define ADS1115_POINTER_CONFIGURATION 0x01
#define ADS1115_POINTER_LO_THRESH 0x02
#define ADS1115_POINTER_HI_THRESH 0x03

// ADS1115 Configuration Register
#define ADS1115_OS_SINGLE 0x8000
#define ADS1115_MUX_MASK 0x7000
#define ADS1115_MUX_AIN0 0x5000
#define ADS1115_MUX_AIN1 0x6000
#define ADS1115_MUX_AIN2 0x7000
#define ADS1115_MUX_AIN3 0x4000
#define ADS1115_PGA_MASK 0x0E00
#define ADS1115_PGA_6_144 0x0000
#define ADS1115_PGA_4_096 0x0200
#define ADS1115_PGA_2_048 0x0400
#define ADS1115_PGA_1_024 0x0600
#define ADS1115_PGA_0_512 0x0800
#define ADS1115_PGA_0_256 0x0A00
#define ADS1115_MODE_MASK 0x0100
#define ADS1115_MODE_CONTINUOUS 0x0000
#define ADS1115_MODE_SINGLE_SHOT 0x0100
#define ADS1115_RATE_MASK 0x00E0
#define ADS1115_RATE_8SPS 0x0000
#define ADS1115_RATE_16SPS 0x0020
#define ADS1115_RATE_32SPS 0x0040
#define ADS1115_RATE_64SPS 0x0060
#define ADS1115_RATE_128SPS 0x0080
#define ADS1115_RATE_250SPS 0x00A0
#define ADS1115_RATE_475SPS 0x00C0
#define ADS1115_RATE_860SPS 0x00E0
#define ADS1115_COMP_MODE_MASK 0x0010
#define ADS1115_COMP_MODE_TRADITIONAL 0x0000
#define ADS1115_COMP_MODE_WINDOW 0x0010
#define ADS1115_COMP_POL_MASK 0x0008
#define ADS1115_COMP_POL_ACTIVE_LOW 0x0000
#define ADS1115_COMP_POL_ACTIVE_HIGH 0x0008
#define ADS1115_COMP_LAT_MASK 0x0004
#define ADS1115_COMP_LAT_NON_LATCHING 0x0000
#define ADS1115_COMP_LAT_LATCHING 0x0004
#define ADS1115_COMP_QUE_MASK 0x0003
#define ADS1115_COMP_QUE_ONE 0x0000
#define ADS1115_COMP_QUE_TWO 0x0001
#define ADS1115_COMP_QUE_FOUR 0x0002
#define ADS1115_COMP_QUE_DISABLE 0x0003

#define ADS1115_MUX_DIFF_0_1 0x0000 // Differential P = AIN0, N = AIN1
#define ADS1115_MUX_DIFF_2_3 0x3000 // Differential P = AIN2, N = AIN3


typedef struct {
    i2c_inst_t *i2c_port;
    uint8_t i2c_sda_pin;
    uint8_t i2c_scl_pin;
    uint8_t address;
    uint16_t config;
} ads1115_t;

void ads1115_init(ads1115_t *adc, i2c_inst_t *i2c_port, uint8_t i2c_sda_pin, uint8_t i2c_scl_pin, uint8_t address);
void ads1115_configure(ads1115_t *adc, uint16_t config);
void ads1115_set_operating_mode(uint16_t mode, ads1115_t *adc);
void ads1115_set_pga(uint16_t pga, ads1115_t *adc);
void ads1115_write_config(ads1115_t *adc);
int16_t ads1115_read(ads1115_t *adc);
void ads1115_read_adc(int16_t *value, ads1115_t *adc);
float ads1115_raw_to_volts(int16_t raw, ads1115_t *adc);
int16_t ads1115_read_channel(ads1115_t *adc, uint8_t channel);
int16_t ads1115_read_differential(ads1115_t *adc, uint8_t channel_pair);

#endif // ADS1115_H