#ifndef L293D_H
#define L293D_H

#include "pico/stdlib.h"

#define NUM_STEPPERS 4

// Define a structure to hold the GPIO pins for the L298N
typedef struct {
    uint pin1;
    uint pin2;
    uint pin3;
    uint pin4;
    int step_index; // Step_index to keep track of the current step
} l293d_t;

typedef struct {
    l293d_t motors[NUM_STEPPERS];
    int motor_count;
} l293d_controller_t;

// Function prototypes
void l293d_init(l293d_t *motor, uint pin1, uint pin2, uint pin3, uint pin4);
void l293d_controller_init(l293d_controller_t *controller, l293d_t motors[], int motor_count);
void l293d_step_forward(l293d_t *motor, uint32_t speed_delay_us, uint32_t steps);
void l293d_step_backward(l293d_t *motor, uint32_t speed_delay_us, uint32_t steps);
void l293d_controller_vector(l293d_controller_t *controller, uint32_t speed_delay_us, int8_t steps[]);
void l293d_stop(l293d_t *motor);

#endif
