#ifndef SHIFT_REGISTER_H
#define SHIFT_REGISTER_H

#include "pico/stdlib.h"

// Define the pins connected to the shift register
#define SR_DATA_PIN 10
#define SR_LATCH_PIN 8
#define SR_CLOCK_PIN 7
#define SR_CLEAR_PIN 6
#define SR_OE_PIN 9

void shift_register_init(void);
void shift_register_write(uint8_t data);
void shift_register_write_16(uint16_t data);
void shift_register_latch(void);
void shift_register_clear(void);
void shift_register_enable_output(bool enable);

#endif