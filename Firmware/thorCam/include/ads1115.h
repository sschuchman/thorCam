#ifndef ADS1115_H
#define ADS1115_H

#include "pico/stdlib.h"
#include "hardware/i2c.h"

// ADS1115 I2C Default Address
#define ADS1115_DEFAULT_ADDRESS 0x48

// Config Register Bits (same as before)
#define ADS1115_OS_START_CONVERSION (1 << 15)
#define ADS1115_MUX_OFFSET 12
#define ADS1115_GAIN_OFFSET 9
#define ADS1115_MODE_SINGLESHOT (1 << 8)
#define ADS1115_DR_OFFSET 5
#define ADS1115_COMP_MODE_TRADITIONAL (0 << 4)
#define ADS1115_COMP_POL_ACTIVE_LOW (0 << 3)
#define ADS1115_COMP_LAT_NON_LATCHING (0 << 2)
#define ADS1115_COMP_QUE_DISABLE (3)

// PGA Settings (2.048V Range)
#define ADS1115_GAIN_OFFSET 9
#define ADS1115_GAIN_6_144V (0 << ADS1115_GAIN_OFFSET) // ±6.144V
#define ADS1115_GAIN_4_096V (1 << ADS1115_GAIN_OFFSET) // ±4.096V
#define ADS1115_GAIN_2_048V (2 << ADS1115_GAIN_OFFSET) // ±2.048V (default)
#define ADS1115_GAIN_1_024V (3 << ADS1115_GAIN_OFFSET) // ±1.024V
#define ADS1115_GAIN_0_512V (4 << ADS1115_GAIN_OFFSET) // ±0.512V
#define ADS1115_GAIN_0_256V (5 << ADS1115_GAIN_OFFSET) // ±0.256V

// Data Rate (860 samples per second)
#define ADS1115_DR_860SPS (7 << ADS1115_DR_OFFSET)

// Mux Settings for Differential Channels
#define ADS1115_MUX_DIFF_0_1 (0 << ADS1115_MUX_OFFSET)
#define ADS1115_MUX_DIFF_2_3 (3 << ADS1115_MUX_OFFSET)

// Mux Settings for Single-ended Channels
#define ADS1115_MUX_SINGLE_0 (4 << ADS1115_MUX_OFFSET)
#define ADS1115_MUX_SINGLE_1 (5 << ADS1115_MUX_OFFSET)
#define ADS1115_MUX_SINGLE_2 (6 << ADS1115_MUX_OFFSET)
#define ADS1115_MUX_SINGLE_3 (7 << ADS1115_MUX_OFFSET)

// ADS1115 instance structure
typedef struct {
    i2c_inst_t *i2c_port;      // Pointer to the I2C interface (e.g., i2c0, i2c1)
    uint8_t i2c_address;       // I2C address of the ADS1115 (e.g., 0x48 or 0x49)
    long diff_0_1_offset;       // Offset for differential reading between AIN0 and AIN1
    long diff_2_3_offset;       // Offset for differential reading between AIN2 and AIN3
} ads1115_t;

// Function Prototypes
void ads1115_init(ads1115_t *adc, i2c_inst_t *i2c_port, uint8_t i2c_address);
void ads1115_calibrate(ads1115_t *adc);
int16_t ads1115_read_diff_0_1(ads1115_t *adc);
int16_t ads1115_read_diff_2_3(ads1115_t *adc);
int16_t ads1115_read_single_channel(ads1115_t *adc, uint8_t channel);
void ads1115_calibrate_0_1(ads1115_t *adc);
void ads1115_calibrate_2_3(ads1115_t *adc);

#endif // ADS1115_H
