/*
    FILE UNUSED IN PROJECT
*/

#include "shift_register.h"

void shift_register_init(void) {
    // Set the shift register pins as outputs
    gpio_init(SR_DATA_PIN);
    gpio_set_dir(SR_DATA_PIN, GPIO_OUT);

    gpio_init(SR_LATCH_PIN);
    gpio_set_dir(SR_LATCH_PIN, GPIO_OUT);

    gpio_init(SR_CLOCK_PIN);
    gpio_set_dir(SR_CLOCK_PIN, GPIO_OUT);

    gpio_init(SR_CLEAR_PIN);
    gpio_set_dir(SR_CLEAR_PIN, GPIO_OUT);

    gpio_init(SR_OE_PIN);
    gpio_set_dir(SR_OE_PIN, GPIO_OUT);

    gpio_put(SR_DATA_PIN, 0);
    gpio_put(SR_LATCH_PIN, 0);
    gpio_put(SR_CLOCK_PIN, 0);
    gpio_put(SR_CLEAR_PIN, 1); // Keep clear high (inactive)
    gpio_put(SR_OE_PIN, 0); // Output enabled by default
}

void shift_register_write(uint8_t data) {
    // Write the data to the shift register
    for (int i = 7; i >= 0; i--) {
        // Set the data pin to the current bit
        gpio_put(SR_DATA_PIN, (data >> i) & 0x01);

        // Pulse the clock pin
        gpio_put(SR_CLOCK_PIN, 1);
        sleep_us(1);
        gpio_put(SR_CLOCK_PIN, 0);
        sleep_us(1);
    }

    // Latch the data
    shift_register_latch();
}

void shift_register_latch(void) {
    // Pulse the latch pin to update the output
    gpio_put(SR_LATCH_PIN, 1);
    sleep_us(1);
    gpio_put(SR_LATCH_PIN, 0);
    sleep_us(1);
}

void shift_register_clear(void) {
    // Clear the shift register
    gpio_put(SR_CLEAR_PIN, 0);
    sleep_us(1);
    gpio_put(SR_CLEAR_PIN, 1);
    sleep_us(1);
}

void shift_register_enable_output(bool enable) {
    // Enable or disable the shift register output
    if(enable) {
        gpio_put(SR_OE_PIN, 0);
    } else {
        gpio_put(SR_OE_PIN, 1);
    }
}

void shift_register_write_16(uint16_t data) {
    // Write the data, starting with the most significant byte
    for (int i = 15; i >= 0; i--) {
        gpio_put(SR_DATA_PIN, (data >> i) & 1);

        // Pulse the clock pin
        gpio_put(SR_CLOCK_PIN, 1);
        sleep_us(1);
        gpio_put(SR_CLOCK_PIN, 0);
        sleep_us(1);
    }

    // Latch the data
    shift_register_latch();
}
