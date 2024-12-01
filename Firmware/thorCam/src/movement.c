#include "movement.h"

l293d_t stepper0, stepper1, stepper2, stepper3;
int8_t vector[] = {1, 1, 1, 1};
l293d_controller_t controller;

void setup_steppers()
{
    // Initialize the steppers
    l293d_init(&stepper0, STEPPER0_PIN1, STEPPER0_PIN2, STEPPER0_PIN3, STEPPER0_PIN4);
    l293d_init(&stepper1, STEPPER1_PIN1, STEPPER1_PIN2, STEPPER1_PIN3, STEPPER1_PIN4);
    l293d_init(&stepper2, STEPPER2_PIN1, STEPPER2_PIN2, STEPPER2_PIN3, STEPPER2_PIN4);
    l293d_init(&stepper3, STEPPER3_PIN1, STEPPER3_PIN2, STEPPER3_PIN3, STEPPER3_PIN4);

    l293d_t steppers[] = {stepper0, stepper1, stepper2, stepper3};
    l293d_controller_init(&controller, steppers, NUM_STEPPERS);
}

/*
    Move 0, -Y
    Forward: 0
    Backward: 2
*/
void move_0(int num_steps, int delay_us)
{

    // Move stepper 2 to lower limit
    while (sensor_values[2] > TENSION_LOWER_LIMIT)
    {
        if(sensor_values[2] < TENSION_LOWER_LIMIT){
            break;
        }

        l293d_step_backward(&stepper2, REVERSE_DELAY, 1);
        sleep_ms(1);
    }

    l293d_stop(&stepper2);

    // Move stepper 0
    if (sensor_values[0] < TENSION_UPPER_LIMIT)
    {
        // Move stepper 0
        l293d_step_forward(&stepper0, delay_us, num_steps);
        sleep_ms(1);
    }
    else
    {
        // printf("Move 0: Stalled\n");
    }
}

/*
    Move -X, 0
    Forward: 0 & 1
    Backward: 2 & 3
*/
void move_1(int num_steps, int delay_us)
{
    // Move steppers 2 and 3 to lower limit
    while (sensor_values[2] > TENSION_LOWER_LIMIT || sensor_values[3] > TENSION_LOWER_LIMIT)
    {
        if(sensor_values[2] < TENSION_LOWER_LIMIT && sensor_values[3] < TENSION_LOWER_LIMIT){
            break;
        }

        if (sensor_values[2] > TENSION_LOWER_LIMIT)
        {
            l293d_step_backward(&stepper2, REVERSE_DELAY, 1);
            sleep_ms(1);
        }

        if (sensor_values[3] > TENSION_LOWER_LIMIT)
        {
            l293d_step_backward(&stepper3, REVERSE_DELAY, 1);
            sleep_ms(1);
        }
    }

    l293d_stop(&stepper2);
    l293d_stop(&stepper3);

    // Move steppers 0 and 1
    if (sensor_values[0] < TENSION_UPPER_LIMIT && sensor_values[1] < TENSION_UPPER_LIMIT)
    {
        // Move both steppers
        // printf("Move 1\n");
        l293d_step_forward(&stepper0, delay_us, num_steps);
        l293d_step_forward(&stepper1, delay_us, num_steps);
        sleep_ms(1);
    }
    else if (sensor_values[0] < TENSION_UPPER_LIMIT)
    {
        // Move stepper 0
        // printf("Move 1: Stepper 1 stalled\n");
        l293d_step_forward(&stepper0, delay_us, num_steps);
        sleep_ms(1);
    }
    else if (sensor_values[1] < TENSION_UPPER_LIMIT)
    {
        // Move stepper 1
        // printf("Move 1: Stepper 0 stalled\n");
        l293d_step_forward(&stepper1, delay_us, num_steps);
        sleep_ms(1);
    }
    else
    {
        // printf("Move 1: Stalled\n");
    }
}

/*
    Move -X, +Y
    Forward: 1
    Backward: 3
*/
void move_2(int num_steps, int delay_us)
{
    // Move stepper 3 to lower limit
    while (sensor_values[3] > TENSION_LOWER_LIMIT)
    {
        if(sensor_values[3] < TENSION_LOWER_LIMIT){
            break;
        }

        l293d_step_backward(&stepper3, REVERSE_DELAY, 1);
        sleep_ms(1);
    }

    l293d_stop(&stepper3);

    // Move stepper 1
    if (sensor_values[1] < TENSION_UPPER_LIMIT)
    {
        // Move stepper 1
        l293d_step_forward(&stepper1, delay_us, num_steps);
        sleep_ms(1);
    }
    else
    {
        // printf("Move 2: Stalled\n");
    }
}

/*
    Move 0, -Y
    Forward: 0 & 3
    Backward: 1 & 2
*/
void move_3(int num_steps, int delay_us)
{
    // Move steppers 1 and 2 to lower limit
    while (sensor_values[1] > TENSION_LOWER_LIMIT || sensor_values[2] > TENSION_LOWER_LIMIT)
    {
        if(sensor_values[1] < TENSION_LOWER_LIMIT && sensor_values[2] < TENSION_LOWER_LIMIT){
            break;
        }

        if (sensor_values[1] > TENSION_LOWER_LIMIT)
        {
            l293d_step_backward(&stepper1, REVERSE_DELAY, 1);
            sleep_ms(1);
        }

        if (sensor_values[2] > TENSION_LOWER_LIMIT)
        {
            l293d_step_backward(&stepper2, REVERSE_DELAY, 1);
            sleep_ms(1);
        }
    }

    l293d_stop(&stepper1);
    l293d_stop(&stepper2);

    // Move steppers 0 and 3
    if (sensor_values[0] < TENSION_UPPER_LIMIT && sensor_values[3] < TENSION_UPPER_LIMIT)
    {
        // Move both steppers
        // printf("Move 3\n");
        l293d_step_forward(&stepper0, delay_us, num_steps);
        l293d_step_forward(&stepper3, delay_us, num_steps);
        sleep_ms(1);
    }
    else if (sensor_values[0] < TENSION_UPPER_LIMIT)
    {
        // Move stepper 0
        // printf("Move 3: Stepper 3 stalled\n");
        l293d_step_forward(&stepper0, delay_us, num_steps);
        sleep_ms(1);
    }
    else if (sensor_values[3] < TENSION_UPPER_LIMIT)
    {
        // Move stepper 3
        // printf("Move 3: Stepper 0 stalled\n");
        l293d_step_forward(&stepper3, delay_us, num_steps);
        sleep_ms(1);
    }
    else
    {
        // printf("Move 3: Stalled\n");
    }
}

/*
    Move 0, 0
    Do nothing
*/
void move_4(int num_steps, int delay_us)
{
    // Do nothing
}

/*
    Move 0, +Y
    Forward: 1 & 2
    Backward: 0 & 3
*/
void move_5(int num_steps, int delay_us)
{
    // Move steppers 0 & 3 to lower limit

    while (sensor_values[0] > TENSION_LOWER_LIMIT || sensor_values[3] > TENSION_LOWER_LIMIT)
    {
        // while(mutex){
        //     printf("Waiting for mutex\n");
        // }

        // // Set the mutex
        // mutex = true;

        if(sensor_values[0] < TENSION_LOWER_LIMIT && sensor_values[3] < TENSION_LOWER_LIMIT){
            break;
        }

        if (sensor_values[0] > TENSION_LOWER_LIMIT)
        {
            l293d_step_backward(&stepper0, REVERSE_DELAY, 1);
            sleep_ms(1);
        }

        if (sensor_values[3] > TENSION_LOWER_LIMIT)
        {
            l293d_step_backward(&stepper3, REVERSE_DELAY, 1);
            sleep_ms(1);
        }

        // // Release the mutex
        // mutex = false;
    }

    l293d_stop(&stepper0);
    l293d_stop(&stepper3);

    // Move steppers 1 and 2
    if (sensor_values[1] < TENSION_UPPER_LIMIT && sensor_values[2] < TENSION_UPPER_LIMIT)
    {
        // Move both steppers
        // printf("Move 5\n");
        l293d_step_forward(&stepper1, delay_us, num_steps);
        l293d_step_forward(&stepper2, delay_us, num_steps);
        sleep_ms(1);
    }
    else if (sensor_values[1] < TENSION_UPPER_LIMIT)
    {
        // Move stepper 1
        // printf("Move 5: Stepper 2 stalled\n");
        l293d_step_forward(&stepper1, delay_us, num_steps);
        sleep_ms(1);
    }
    else if (sensor_values[2] < TENSION_UPPER_LIMIT)
    {
        // Move stepper 2
        // printf("Move 5: Stepper 1 stalled\n");
        l293d_step_forward(&stepper2, delay_us, num_steps);
        sleep_ms(1);
    }
    else
    {
        // printf("Move 5: Stalled\n");
    }
}

/*
    Move +X, -Y
    Forward: 3
    Backward: 1
*/
void move_6(int num_steps, int delay_us)
{
    // Move stepper 1 to lower limit
    while (sensor_values[1] > TENSION_LOWER_LIMIT)
    {
        if(sensor_values[1] < TENSION_LOWER_LIMIT){
            break;
        }

        l293d_step_backward(&stepper1, REVERSE_DELAY, 1);
        sleep_ms(1);
    }

    l293d_stop(&stepper1);

    // Move stepper 3
    if (sensor_values[3] < TENSION_UPPER_LIMIT)
    {
        // Move stepper 3
        l293d_step_forward(&stepper3, delay_us, num_steps);
        sleep_ms(1);
    }
    else
    {
        // printf("Move 6: Stalled\n");
    }
}

/*
    Move +X, 0
    Forward: 2 & 3
    Backward: 0 & 1
*/
void move_7(int num_steps, int delay_us)
{
    // Move steppers 0 and 1 to lower limit
    while (sensor_values[0] > TENSION_LOWER_LIMIT || sensor_values[1] > TENSION_LOWER_LIMIT)
    {
        if(sensor_values[0] < TENSION_LOWER_LIMIT && sensor_values[1] < TENSION_LOWER_LIMIT){
            break;
        }

        if (sensor_values[0] > TENSION_LOWER_LIMIT)
        {
            l293d_step_backward(&stepper0, REVERSE_DELAY, 1);
            sleep_ms(1);
        }

        if (sensor_values[1] > TENSION_LOWER_LIMIT)
        {
            l293d_step_backward(&stepper1, REVERSE_DELAY, 1);
            sleep_ms(1);
        }
    }

    l293d_stop(&stepper0);
    l293d_stop(&stepper1);

    // Move steppers 2 and 3
    if (sensor_values[2] < TENSION_UPPER_LIMIT && sensor_values[3] < TENSION_UPPER_LIMIT)
    {
        // Move both steppers
        // printf("Move 7\n");
        l293d_step_forward(&stepper2, delay_us, num_steps);
        l293d_step_forward(&stepper3, delay_us, num_steps);
        sleep_ms(1);
    }
    else if (sensor_values[2] < TENSION_UPPER_LIMIT)
    {
        // Move stepper 2
        // printf("Move 7: Stepper 3 stalled\n");
        l293d_step_forward(&stepper2, delay_us, num_steps);
        sleep_ms(1);
    }
    else if (sensor_values[3] < TENSION_UPPER_LIMIT)
    {
        // Move stepper 3
        // printf("Move 7: Stepper 2 stalled\n");
        l293d_step_forward(&stepper3, delay_us, num_steps);
        sleep_ms(1);
    }
    else
    {
        // printf("Move 7: Stalled\n");
    }
}

/*
    Move +X, +Y
    Forward: 2
    Backward: 0
*/
void move_8(int num_steps, int delay_us)
{
    // Move stepper 0 to lower limit
    while (sensor_values[0] > TENSION_LOWER_LIMIT)
    {
        if(sensor_values[0] < TENSION_LOWER_LIMIT){
            break;
        }

        l293d_step_backward(&stepper0, REVERSE_DELAY, 1);
        sleep_ms(1);
    }

    l293d_stop(&stepper0);

    // Move stepper 2
    if (sensor_values[2] < TENSION_UPPER_LIMIT)
    {
        // Move stepper 2
        l293d_step_forward(&stepper2, delay_us, num_steps);
        sleep_ms(1);
    }
    else
    {
        // printf("Move 8: Stalled\n");
    }
}

void move(int num_steps, int delay_us, int x, int y)
{
    // Combine x and y into a single move code
    volatile int move_code = (x + 1) * 3 + (y + 1);

    for (int i = 0; i < num_steps; i++)
    {
        switch (move_code)
        {
        case 0:
            // Move to -X, -Y
            move_0(1, delay_us);
            break;
        case 1:
            // Move to -X, 0
            move_1(1, delay_us);
            break;
        case 2:
            // Move to -X, +Y
            move_2(1, delay_us);
            break;
        case 3:
            // Move to 0, -Y
            move_3(1, delay_us);
            break;
        case 4:
            // Move to 0, 0
            move_4(1, delay_us);
            break;
        case 5:
            // Move to 0, +Y
            move_5(1, delay_us);
            break;
        case 6:
            // Move to +X, -Y
            move_6(1, delay_us);
            break;
        case 7:
            // Move to +X, 0
            move_7(1, delay_us);
            break;
        case 8:
            // Move to +X, +Y
            move_8(1, delay_us);
            break;
        default:
            // Invalid move code
            printf("Invalid move code\n");
            break;
        }
    }

    /*
    Move codes:
        0: Move to -X, -Y
            F: 0
            B: 2
        1: Move to -X, 0
            F: 0 & 1
            B: 2 & 3
        2: Move to -X, +Y
            F: 1
            B: 3
        3: Move to 0, -Y
            F: 0 & 3
            B: 1 & 2
        4: Move to 0, 0
            Do nothing
        5: Move to 0, +Y
            F: 1 & 2
            B: 0 & 3
        6: Move to +X, -Y
            F: 3
            B: 1
        7: Move to +X, 0
            F: 2 & 3
            B: 0 & 1
        8: Move to +X, +Y
            F: 2
            B: 0
*/
}
