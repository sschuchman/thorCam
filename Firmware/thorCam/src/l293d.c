#include "l293d.h"
#include <stdio.h>

/*
    Stepper coil control
    A: {0, 0, 0, 1}
    B: {0, 0, 1, 0}
    A': {0, 1, 0, 0}
    B': {1, 0, 0, 0}
*/

// Define the step sequences for a bipolar stepper motor
static const uint8_t step_sequence[4][4] = {
    {1, 0, 0, 1}, // B'
    {1, 1, 0, 0}, // A'
    {0, 1, 1, 0}, // B
    {0, 0, 1, 1}  // A
};

// Initialize the GPIO pins for the L298N
void l293d_init(l293d_t *motor, uint pin1, uint pin2, uint pin3, uint pin4)
{
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

void l293d_controller_init(l293d_controller_t *controller, l293d_t motors[], int motor_count)
{
    for (int i = 0; i < 4; i++)
    {
        controller->motors[i] = motors[i];
    }
}

// Step the motor forward with adjustable speed and distance using microstepping
void l293d_step_forward(l293d_t *motor, uint32_t speed_delay_us, uint32_t steps)
{
    for (uint32_t i = 0; i < steps; i++)
    { // Multiply steps by 8 for microstepping
        gpio_put(motor->pin1, step_sequence[motor->step_index][0]);
        gpio_put(motor->pin2, step_sequence[motor->step_index][1]);
        gpio_put(motor->pin3, step_sequence[motor->step_index][2]);
        gpio_put(motor->pin4, step_sequence[motor->step_index][3]);
        motor->step_index = (motor->step_index + 1) % 4; // Advance to the next microstep

        // printf("Step index: %d\n", motor->step_index);
        // printf("Step sequence: [%d, %d, %d, %d]\n", step_sequence[motor->step_index][0], step_sequence[motor->step_index][1], step_sequence[motor->step_index][2], step_sequence[motor->step_index][3]);

        sleep_us(speed_delay_us); // Adjust speed by changing this delay
    }

    //l293d_stop(motor);
}

// Step the motor backward with adjustable speed and distance using microstepping
void l293d_step_backward(l293d_t *motor, uint32_t speed_delay_us, uint32_t steps)
{
    for (uint32_t i = 0; i < steps; i++)
    { // Multiply steps by 8 for microstepping
        gpio_put(motor->pin1, step_sequence[motor->step_index][0]);
        gpio_put(motor->pin2, step_sequence[motor->step_index][1]);
        gpio_put(motor->pin3, step_sequence[motor->step_index][2]);
        gpio_put(motor->pin4, step_sequence[motor->step_index][3]);
        motor->step_index = (motor->step_index - 1 + 4) % 4; // Advance to the next microstep
        // printf("Step index: %d\n", motor->step_index);
        sleep_us(speed_delay_us); // Adjust speed by changing this delay
    }

    //l293d_stop(motor);
}

void l293d_controller_vector(l293d_controller_t *controller, uint32_t speed_delay_us, int8_t steps[])
{
    for (int i = 0; i < 4; i++)
    {
        if (steps[i] > 0)
        {
            // printf("Stepping forward %d steps\n", steps[i]);
            l293d_step_forward(&controller->motors[i], speed_delay_us, steps[i]);
        }
        else if (steps[i] < 0)
        {
            // printf("Stepping backward %d steps\n", -steps[i]);
            l293d_step_backward(&controller->motors[i], speed_delay_us, -steps[i]);
        }
        else
        {
            // printf("Stopping motor\n");
            // l293d_stop(&controller->motors[i]);
        }
    }
}

// Stop the motor
void l293d_stop(l293d_t *motor)
{
    gpio_put(motor->pin1, 0);
    gpio_put(motor->pin2, 0);
    gpio_put(motor->pin3, 0);
    gpio_put(motor->pin4, 0);
}
