#include "ads1115.h"
#include "hardware/i2c.h"
#include "pico/stdlib.h"
#include <stdio.h> // Include the stdio.h header for printf

// Initialize the ADS1115
void ads1115_init(ads1115_t *adc, i2c_inst_t *i2c_port, uint8_t i2c_sda_pin, uint8_t i2c_scl_pin, uint8_t address) {
    adc->i2c_port = i2c_port;
    adc->i2c_sda_pin = i2c_sda_pin;
    adc->i2c_scl_pin = i2c_scl_pin;
    adc->address = address;

    // Initialize I2C
    i2c_init(adc->i2c_port, 400 * 1000);
    gpio_set_function(adc->i2c_sda_pin, GPIO_FUNC_I2C);
    gpio_set_function(adc->i2c_scl_pin, GPIO_FUNC_I2C);
    gpio_pull_up(adc->i2c_sda_pin);
    gpio_pull_up(adc->i2c_scl_pin);
}

// Configure the ADS1115
void ads1115_configure(ads1115_t *adc, uint16_t config) {
    adc->config = config;
    ads1115_write_config(adc);
}

// Set the operating mode
void ads1115_set_operating_mode(uint16_t mode, ads1115_t *adc) {
    adc->config = (adc->config & ~ADS1115_MODE_MASK) | mode;
}

// Set the programmable gain amplifier
void ads1115_set_pga(uint16_t pga, ads1115_t *adc) {
    adc->config = (adc->config & ~ADS1115_PGA_MASK) | pga;
}

// Write the configuration to the ADS1115
void ads1115_write_config(ads1115_t *adc) {
    uint8_t buffer[3];
    buffer[0] = ADS1115_POINTER_CONFIGURATION; // Pointer to config register
    buffer[1] = adc->config >> 8;
    buffer[2] = adc->config & 0xFF;
    int ret = i2c_write_blocking(adc->i2c_port, adc->address, buffer, 3, false);
    if (ret < 0) {
        printf("Failed to write to ADS1115 at address 0x%02x\n", adc->address);
    } else {
        //printf("ADS1115 configured at address 0x%02x\n", adc->address);
    }
}

// Read a value from the ADS1115
int16_t ads1115_read(ads1115_t *adc) {
    uint8_t buffer[2];
    uint8_t pointer = ADS1115_POINTER_CONVERSION; // Pointer to conversion register
    int ret = i2c_write_blocking(adc->i2c_port, adc->address, &pointer, 1, false);
    if (ret < 0) {
        printf("Failed to write to ADS1115 at address 0x%02x\n", adc->address);
        return -1;
    }
    ret = i2c_read_blocking(adc->i2c_port, adc->address, buffer, 2, false);
    if (ret < 0) {
        printf("Failed to read from ADS1115 at address 0x%02x\n", adc->address);
        return -1;
    }
    return (buffer[0] << 8) | buffer[1];
}

// Read ADC value
void ads1115_read_adc(int16_t *value, ads1115_t *adc) {
    *value = ads1115_read(adc);
}

// Convert raw ADC value to voltage
float ads1115_raw_to_volts(int16_t raw, ads1115_t *adc) {
    float pga = 2.048; // Default PGA value
    switch (adc->config & ADS1115_PGA_MASK) {
        case ADS1115_PGA_6_144: pga = 6.144; break;
        case ADS1115_PGA_4_096: pga = 4.096; break;
        case ADS1115_PGA_2_048: pga = 2.048; break;
        case ADS1115_PGA_1_024: pga = 1.024; break;
        case ADS1115_PGA_0_512: pga = 0.512; break;
        case ADS1115_PGA_0_256: pga = 0.256; break;
    }
    return (raw * pga) / 32768.0;
}

// Read a specific channel or differential input from the ADS1115
int16_t ads1115_read_channel(ads1115_t *adc, uint8_t channel) {
    switch (channel) {
        case 0:
            adc->config = (adc->config & ~ADS1115_MUX_MASK) | ADS1115_MUX_AIN0;
            break;
        case 1:
            adc->config = (adc->config & ~ADS1115_MUX_MASK) | ADS1115_MUX_AIN1;
            break;
        case 2:
            adc->config = (adc->config & ~ADS1115_MUX_MASK) | ADS1115_MUX_AIN2;
            break;
        case 3:
            adc->config = (adc->config & ~ADS1115_MUX_MASK) | ADS1115_MUX_AIN3;
            break;
        default:
            printf("Invalid channel: %d\n", channel);
            return -1;
    }

    // Set the OS bit to start a single conversion
    adc->config |= ADS1115_OS_SINGLE;
    ads1115_write_config(adc);
    return ads1115_read(adc);
}

// Read differential voltage between specified channel pairs (0-1 and 2-3)
int16_t ads1115_read_differential(ads1115_t *adc, uint8_t channel_pair) {
    switch (channel_pair) {
        case 0:
            // Set MUX for differential reading between AIN0 and AIN1
            adc->config = (adc->config & ~ADS1115_MUX_MASK) | ADS1115_MUX_DIFF_0_1;
            break;
        case 1:
            // Set MUX for differential reading between AIN2 and AIN3
            adc->config = (adc->config & ~ADS1115_MUX_MASK) | ADS1115_MUX_DIFF_2_3;
            break;
        default:
            printf("Invalid channel pair: %d\n", channel_pair);
            return -1;
    }

    // Set the OS bit to start a single conversion
    adc->config |= ADS1115_OS_SINGLE;
    ads1115_write_config(adc);
    return ads1115_read(adc);
}
