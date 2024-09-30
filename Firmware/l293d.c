#include "l293d.h"

// Define the step sequences for a bipolar stepper motor
static const uint8_t microstep_sequence[8][4] = {
    {1, 0, 1, 0}, // Step 1: A+ B-
    {1, 0, 1, 1}, // Step 2: A+ B- (half power)
    {1, 0, 0, 0}, // Step 3: A+ B0
    {1, 1, 0, 1}, // Step 4: A+ B+ (half power)
    {1, 1, 0, 1}, // Step 5: A+ B+
    {0, 1, 0, 1}, // Step 6: A- B+ (half power)
    {0, 0, 0, 1}, // Step 7: A0 B+
    {0, 1, 1, 1}  // Step 8: A- B+ (half power)
};

// Initialize the GPIO pins for the L298N
void l293d_init(l293d_t *motor, uint pin1, uint pin2, uint pin3, uint pin4) {
    motor->pin1 = pin1;
    motor->pin2 = pin2;
    motor->pin3 = pin3;
    motor->pin4 = pin4;
    motor->step_index = 0; // Initialize step index

    gpio_init(pin1);
    gpio_set_dir(pin1, GPIO_OUT);
    gpio_init(pin2);
    gpio_set_dir(pin2, GPIO_OUT);
    gpio_init(pin3);
    gpio_set_dir(pin3, GPIO_OUT);
    gpio_init(pin4);
    gpio_set_dir(pin4, GPIO_OUT);
}

// Step the motor forward with adjustable speed and distance using microstepping
void l293d_step_forward(l293d_t *motor, uint32_t speed_delay_us, uint32_t steps) {
    for (uint32_t i = 0; i < steps * 8; i++) { // Multiply steps by 8 for microstepping
        gpio_put(motor->pin1, microstep_sequence[motor->step_index][0]);
        gpio_put(motor->pin2, microstep_sequence[motor->step_index][1]);
        gpio_put(motor->pin3, microstep_sequence[motor->step_index][2]);
        gpio_put(motor->pin4, microstep_sequence[motor->step_index][3]);
        motor->step_index = (motor->step_index + 1) % 8; // Advance to the next microstep
        sleep_us(speed_delay_us); // Adjust speed by changing this delay
    }

    l293d_stop(motor);
}

// Step the motor backward with adjustable speed and distance using microstepping
void l293d_step_backward(l293d_t *motor, uint32_t speed_delay_us, uint32_t steps) {
    for (uint32_t i = 0; i < steps * 8; i++) { // Multiply steps by 8 for microstepping
        gpio_put(motor->pin1, microstep_sequence[motor->step_index][0]);
        gpio_put(motor->pin2, microstep_sequence[motor->step_index][1]);
        gpio_put(motor->pin3, microstep_sequence[motor->step_index][2]);
        gpio_put(motor->pin4, microstep_sequence[motor->step_index][3]);
        motor->step_index = (motor->step_index - 1 + 8) % 8; // Move to the previous microstep
        sleep_us(speed_delay_us); // Adjust speed by changing this delay
    }

    l293d_stop(motor);
}

// Stop the motor
void l293d_stop(l293d_t *motor) {
    gpio_put(motor->pin1, 0);
    gpio_put(motor->pin2, 0);
    gpio_put(motor->pin3, 0);
    gpio_put(motor->pin4, 0);
}
