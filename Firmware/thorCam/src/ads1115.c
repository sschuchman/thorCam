#include "ads1115.h"
#include <stdio.h>

static void ads1115_write_register(ads1115_t *adc, uint8_t reg, uint16_t value) {
    uint8_t buffer[3];
    buffer[0] = reg;
    buffer[1] = value >> 8;   // MSB
    buffer[2] = value & 0xFF; // LSB
    i2c_write_blocking(adc->i2c_port, adc->i2c_address, buffer, 3, false);
}

static uint16_t ads1115_read_register(ads1115_t *adc, uint8_t reg) {
    uint8_t buffer[2];
    i2c_write_blocking(adc->i2c_port, adc->i2c_address, &reg, 1, true); // Request register
    i2c_read_blocking(adc->i2c_port, adc->i2c_address, buffer, 2, false); // Read register
    return (buffer[0] << 8) | buffer[1]; // Combine MSB and LSB
}

void ads1115_init(ads1115_t *adc, i2c_inst_t *i2c_port, uint8_t i2c_address) {
    adc->i2c_port = i2c_port;
    adc->i2c_address = i2c_address;
}

static int16_t ads1115_read_adc(ads1115_t *adc, uint16_t config) {
    // Write to the configuration register to start conversion
    config |= ADS1115_OS_START_CONVERSION; // Start a single conversion
    config |= ADS1115_GAIN_0_256V;         // Set gain to ±0.256V
    config |= ADS1115_DR_860SPS;           // Set data rate
    config |= ADS1115_MODE_SINGLESHOT;     // Set to single-shot mode
    config |= ADS1115_COMP_QUE_DISABLE;    // Disable comparator

    ads1115_write_register(adc, 0x01, config);

    // Wait for conversion to complete
    sleep_ms(2); // Rough wait for conversion (depends on data rate)

    // Read conversion result
    return (int16_t)ads1115_read_register(adc, 0x00); // Conversion register
}

int16_t ads1115_read_diff_0_1(ads1115_t *adc) {
    uint16_t config = ADS1115_MUX_DIFF_0_1; // Set for differential between AIN0 and AIN1
    return ads1115_read_adc(adc, config) - adc->diff_0_1_offset;
}

int16_t ads1115_read_diff_2_3(ads1115_t *adc) {
    uint16_t config = ADS1115_MUX_DIFF_2_3; // Set for differential between AIN2 and AIN3
    return ads1115_read_adc(adc, config) - adc->diff_2_3_offset;
}

int16_t ads1115_read_single_channel(ads1115_t *adc, uint8_t channel) {
    uint16_t config = 0;
    switch (channel) {
        case 0:
            config = ADS1115_MUX_SINGLE_0;
            break;
        case 1:
            config = ADS1115_MUX_SINGLE_1;
            break;
        case 2:
            config = ADS1115_MUX_SINGLE_2;
            break;
        case 3:
            config = ADS1115_MUX_SINGLE_3;
            break;
        default:
            return 0; // Invalid channel
    }
    return ads1115_read_adc(adc, config);
}

void ads1115_calibrate(ads1115_t *adc) {
    uint16_t config = ADS1115_MUX_DIFF_0_1; // Set for differential between AIN0 and AIN1
    uint16_t diff_0_1 = ads1115_read_adc(adc, config);

    config = ADS1115_MUX_DIFF_2_3; // Set for differential between AIN2 and AIN3
    uint16_t diff_2_3 = ads1115_read_adc(adc, config);

    if(diff_0_1 > 500 || diff_0_1 < -500){
        diff_0_1 = 0;
    }

    if(diff_2_3 > 500 || diff_2_3 < -500){
        diff_2_3 = 0;
    }

    adc->diff_0_1_offset = diff_0_1;
    adc->diff_2_3_offset = diff_2_3;
}

void ads1115_calibrate_0_1(ads1115_t *adc) {
    uint16_t config = ADS1115_MUX_DIFF_0_1; // Set for differential between AIN0 and AIN1
    uint16_t diff_0_1 = ads1115_read_adc(adc, config);

    if(diff_0_1 > 500 || diff_0_1 < -500){
        diff_0_1 = 0;
    }

    adc->diff_0_1_offset = diff_0_1;

    printf("Offset: %d\n", adc->diff_0_1_offset);
}

void ads1115_calibrate_2_3(ads1115_t *adc) {
    uint16_t config = ADS1115_MUX_DIFF_2_3; // Set for differential between AIN2 and AIN3
    uint16_t diff_2_3 = ads1115_read_adc(adc, config);

    if(diff_2_3 > 500 || diff_2_3 < -500){
        diff_2_3 = 0;
    }

    adc->diff_2_3_offset = diff_2_3;

    printf("Offset: %d\n", adc->diff_2_3_offset);
}
