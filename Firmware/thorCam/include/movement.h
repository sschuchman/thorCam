#ifndef MOVEMENT_H
#define MOVEMENT_H

#include "sensor.h"
#include <stdio.h>
#include "pico/stdlib.h"
#include "l293d.h"

#define STEPPER0_PIN1 10
#define STEPPER0_PIN2 12
#define STEPPER0_PIN3 11
#define STEPPER0_PIN4 13

#define STEPPER1_PIN1 14
#define STEPPER1_PIN2 16
#define STEPPER1_PIN3 15
#define STEPPER1_PIN4 17

#define STEPPER2_PIN1 18
#define STEPPER2_PIN2 20
#define STEPPER2_PIN3 19
#define STEPPER2_PIN4 21

#define STEPPER3_PIN1 22
#define STEPPER3_PIN2 27
#define STEPPER3_PIN3 26
#define STEPPER3_PIN4 28

#define NUM_STEPPERS 4

#define TENSION_HARD_UPPER_LIMIT 300
#define TENSION_UPPER_LIMIT 250
#define TENSION_OPTIMAL 100
#define TENSION_LOWER_LIMIT 25

#define REVERSE_DELAY 1000
#define FORWARD_DELAY 1000

extern l293d_t stepper0, stepper1, stepper2, stepper3;
extern l293d_controller_t controller;
extern int8_t vector[];

// Function prototypes
void calibrate();
void setup_steppers();
void move(int num_steps, int delay_us, int x, int y);

// Movement sub-functions
void move_0(int num_steps, int delay_us);
void move_1(int num_steps, int delay_us);
void move_2(int num_steps, int delay_us);
void move_3(int num_steps, int delay_us);
void move_4(int num_steps, int delay_us);
void move_5(int num_steps, int delay_us);
void move_6(int num_steps, int delay_us);
void move_7(int num_steps, int delay_us);
void move_8(int num_steps, int delay_us);

#endif // MOVEMENT_H
