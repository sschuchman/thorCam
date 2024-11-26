#include "pico/stdlib.h"
#include "l293d.h"
#include <stdio.h>
#include "ads1115.h"
#include "hardware/i2c.h"
#include "pico/multicore.h"
#include "shift_register.h"

#define LED_PIN 25 // Onboard LED pin
// #define LOAD_1_DOUT 21 // GPIO pin for HX711 DOUT
// #define LOAD_1_SCK 22  // GPIO pin for HX711 SCK
// #define LOAD_2_DOUT 26 // GPIO pin for HX711 DOUT
// #define LOAD_2_SCK 27  // GPIO pin for HX711 SCK

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

#define ONBOARD_LED 25

#define NUM_STEPPERS 4

#define I2C_PORT i2c0
#define I2C_SCL 5
#define I2C_SDA 4

l293d_t stepper0, stepper1, stepper2, stepper3;
l293d_controller_t controller;

ads1115_t adc1, adc2;
int16_t diff_0_1_adc1, diff_2_3_adc1, diff_0_1_adc2, diff_2_3_adc2;

#define TENSION_HARD_UPPER_LIMIT 300
#define TENSION_UPPER_LIMIT 260
#define TENSION_OPTIMAL 100
#define TENSION_LOWER_LIMIT 10

#define false 0
#define true 1

volatile bool mutex = false;
volatile bool stop_flag = false;
int16_t sensor_values[4] = {0, 0, 0, 0};

int8_t vector[] = {1, 1, 1, 1};

#define REVERSE_DELAY 1000
#define FORWARD_DELAY 1000

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

void read_ads111_task()
{
    while (true)
    {
        // Read from the first ADS1115 (address 0x48)
        diff_0_1_adc1 = ads1115_read_diff_0_1(&adc1);
        // printf("ADC1 (0x48) Differential between AIN0 and AIN1: %d\n", diff_0_1_adc1);
        diff_2_3_adc1 = ads1115_read_diff_2_3(&adc1);
        // printf("ADC1 (0x48) Differential between AIN2 and AIN3: %d\n", diff_2_3_adc1);

        // Read from the second ADS1115 (address 0x49)
        diff_0_1_adc2 = ads1115_read_diff_0_1(&adc2);
        // printf("ADC2 (0x49) Differential between AIN0 and AIN1: %d\n", diff_0_1_adc2);
        diff_2_3_adc2 = ads1115_read_diff_2_3(&adc2);
        // printf("ADC2 (0x49) Differential between AIN2 and AIN3: %d\n", diff_2_3_adc2);

        // Wait for mutex to be released
        while (mutex)
        {
            printf("Waiting for mutex to be released\n");
        };

        // Set mutex
        mutex = true;
        // Turn on ONBOARD LED
        gpio_put(ONBOARD_LED, 1);

        // Update diff_array
        sensor_values[0] = (diff_0_1_adc1 > 0) ? diff_0_1_adc1 : 0;
        sensor_values[1] = (diff_2_3_adc1 > 0) ? diff_2_3_adc1 : 0;
        sensor_values[2] = (diff_0_1_adc2 > 0) ? diff_0_1_adc2 : 0;
        sensor_values[3] = (diff_2_3_adc2 > 0) ? diff_2_3_adc2 : 0;

        // Release mutex
        mutex = false;

        // Turn off ONBOARD LED
        gpio_put(ONBOARD_LED, 0);

        printf("Load[%d, %d, %d, %d]\n", sensor_values[0], sensor_values[1], sensor_values[2], sensor_values[3]);

        sleep_ms(100);

        // char command[100];

        // if (fgets(command, sizeof(command), stdin) != NULL)
        // {
        //     // Remove newline character if present
        //     command[strcspn(command, "\n")] = 0;
        //     if (strcmp(command, "stop") == 0)
        //     {
        //         stop_flag = true;
        //         break;
        //     }
        // }
    }
}

void calibrate()
{
    printf("Calibrating ADS1115s...\n");

    int unwinding_delay = 5000;
    int unwinding_steps = 200;

    l293d_stop(&stepper0);
    l293d_stop(&stepper1);
    l293d_stop(&stepper2);
    l293d_stop(&stepper3);

    printf("Calibrating stepper 0...\n");
    // Unwind stepper0
    l293d_step_backward(&stepper0, unwinding_delay, unwinding_steps);
    sleep_ms(1000);

    // Calibrate the ADS1115s
    ads1115_calibrate_0_1(&adc1);

    // Wind until tension is optimal
    while (diff_0_1_adc1 < TENSION_LOWER_LIMIT)
    {
        l293d_step_forward(&stepper0, 5000, 1);
        diff_0_1_adc1 = ads1115_read_diff_0_1(&adc1);
    }

    // Stop the stepper
    l293d_stop(&stepper0);

    printf("Calibrating stepper 1...\n");
    // Unwind stepper1
    l293d_step_backward(&stepper1, unwinding_delay, unwinding_steps);
    sleep_ms(1000);

    // Calibrate the ADS1115s
    ads1115_calibrate_2_3(&adc1);

    // Wind until tension is optimal
    while (diff_2_3_adc1 < TENSION_LOWER_LIMIT)
    {
        l293d_step_forward(&stepper1, 5000, 1);
        diff_2_3_adc1 = ads1115_read_diff_2_3(&adc1);
    }

    // Stop the stepper
    l293d_stop(&stepper1);

    printf("Calibrating stepper 2...\n");
    // Unwind stepper2
    l293d_step_backward(&stepper2, unwinding_delay, unwinding_steps);
    sleep_ms(1000);

    // Calibrate the ADS1115s
    ads1115_calibrate_0_1(&adc2);

    // Wind until tension is optimal
    while (diff_0_1_adc2 < TENSION_LOWER_LIMIT)
    {
        l293d_step_forward(&stepper2, 5000, 1);
        diff_0_1_adc2 = ads1115_read_diff_0_1(&adc2);
    }

    // Stop the stepper
    l293d_stop(&stepper2);

    printf("Calibrating stepper 3...\n");
    // Unwind stepper3
    l293d_step_backward(&stepper3, unwinding_delay, unwinding_steps);
    sleep_ms(1000);

    // Calibrate the ADS1115s
    ads1115_calibrate_2_3(&adc2);

    // Wind until tension is optimal
    while (diff_2_3_adc2 < TENSION_LOWER_LIMIT)
    {
        l293d_step_forward(&stepper3, 5000, 1);
        diff_2_3_adc2 = ads1115_read_diff_2_3(&adc2);
    }

    // Stop the stepper
    l293d_stop(&stepper3);

    printf("Calibration complete\n");
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
        l293d_step_backward(&stepper2, REVERSE_DELAY, 1);
    }

    l293d_stop(&stepper2);

    // Move stepper 0
    if(sensor_values[0] < TENSION_UPPER_LIMIT)
    {
        // Move stepper 0
        l293d_step_forward(&stepper0, delay_us, num_steps);
    }
    else{
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
        if (sensor_values[2] > TENSION_LOWER_LIMIT)
        {
            l293d_step_backward(&stepper2, REVERSE_DELAY, 1);
        }

        if (sensor_values[3] > TENSION_LOWER_LIMIT)
        {
            l293d_step_backward(&stepper3, REVERSE_DELAY, 1);
        }
    }

    l293d_stop(&stepper2);
    l293d_stop(&stepper3);

    // Move steppers 0 and 1
    if(sensor_values[0] < TENSION_UPPER_LIMIT && sensor_values[1] < TENSION_UPPER_LIMIT)
    {
        // Move both steppers
        // printf("Move 1\n");
        l293d_step_forward(&stepper0, delay_us, num_steps);
        l293d_step_forward(&stepper1, delay_us, num_steps);
    }
    else if(sensor_values[0] < TENSION_UPPER_LIMIT)
    {
        // Move stepper 0
        // printf("Move 1: Stepper 1 stalled\n");
        l293d_step_forward(&stepper0, delay_us, num_steps);
    }
    else if(sensor_values[1] < TENSION_UPPER_LIMIT)
    {
        // Move stepper 1
        // printf("Move 1: Stepper 0 stalled\n");
        l293d_step_forward(&stepper1, delay_us, num_steps);
    }
    else{
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
        l293d_step_backward(&stepper3, REVERSE_DELAY, 1);
    }

    l293d_stop(&stepper3);

    // Move stepper 1
    if(sensor_values[1] < TENSION_UPPER_LIMIT)
    {
        // Move stepper 1
        l293d_step_forward(&stepper1, delay_us, num_steps);
    }
    else{
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
        if (sensor_values[1] > TENSION_LOWER_LIMIT)
        {
            l293d_step_backward(&stepper1, REVERSE_DELAY, 1);
        }

        if (sensor_values[2] > TENSION_LOWER_LIMIT)
        {
            l293d_step_backward(&stepper2, REVERSE_DELAY, 1);
        }
    }

    l293d_stop(&stepper1);
    l293d_stop(&stepper2);

    // Move steppers 0 and 3
    if(sensor_values[0] < TENSION_UPPER_LIMIT && sensor_values[3] < TENSION_UPPER_LIMIT)
    {
        // Move both steppers
        // printf("Move 3\n");
        l293d_step_forward(&stepper0, delay_us, num_steps);
        l293d_step_forward(&stepper3, delay_us, num_steps);
    }
    else if(sensor_values[0] < TENSION_UPPER_LIMIT)
    {
        // Move stepper 0
        // printf("Move 3: Stepper 3 stalled\n");
        l293d_step_forward(&stepper0, delay_us, num_steps);
    }
    else if(sensor_values[3] < TENSION_UPPER_LIMIT)
    {
        // Move stepper 3
        // printf("Move 3: Stepper 0 stalled\n");
        l293d_step_forward(&stepper3, delay_us, num_steps);
    }
    else{
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
        if (sensor_values[0] > TENSION_LOWER_LIMIT)
        {
            l293d_step_backward(&stepper0, REVERSE_DELAY, 1);
        }

        if (sensor_values[3] > TENSION_LOWER_LIMIT)
        {
            l293d_step_backward(&stepper3, REVERSE_DELAY, 1);
        }
    }

    l293d_stop(&stepper0);
    l293d_stop(&stepper3);

    // Move steppers 1 and 2
    if(sensor_values[1] < TENSION_UPPER_LIMIT && sensor_values[2] < TENSION_UPPER_LIMIT)
    {
        // Move both steppers
        // printf("Move 5\n");
        l293d_step_forward(&stepper1, delay_us, num_steps);
        l293d_step_forward(&stepper2, delay_us, num_steps);
    }
    else if(sensor_values[1] < TENSION_UPPER_LIMIT)
    {
        // Move stepper 1
        // printf("Move 5: Stepper 2 stalled\n");
        l293d_step_forward(&stepper1, delay_us, num_steps);
    }
    else if(sensor_values[2] < TENSION_UPPER_LIMIT)
    {
        // Move stepper 2
        // printf("Move 5: Stepper 1 stalled\n");
        l293d_step_forward(&stepper2, delay_us, num_steps);
    }
    else{
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
        l293d_step_backward(&stepper1, REVERSE_DELAY, 1);
    }

    l293d_stop(&stepper1);

    // Move stepper 3
    if(sensor_values[3] < TENSION_UPPER_LIMIT)
    {
        // Move stepper 3
        l293d_step_forward(&stepper3, delay_us, num_steps);
    }
    else{
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
        if (sensor_values[0] > TENSION_LOWER_LIMIT)
        {
            l293d_step_backward(&stepper0, REVERSE_DELAY, 1);
        }

        if (sensor_values[1] > TENSION_LOWER_LIMIT)
        {
            l293d_step_backward(&stepper1, REVERSE_DELAY, 1);
        }
    }

    l293d_stop(&stepper0);
    l293d_stop(&stepper1);

    // Move steppers 2 and 3
    if(sensor_values[2] < TENSION_UPPER_LIMIT && sensor_values[3] < TENSION_UPPER_LIMIT)
    {
        // Move both steppers
        // printf("Move 7\n");
        l293d_step_forward(&stepper2, delay_us, num_steps);
        l293d_step_forward(&stepper3, delay_us, num_steps);
    }
    else if(sensor_values[2] < TENSION_UPPER_LIMIT)
    {
        // Move stepper 2
        // printf("Move 7: Stepper 3 stalled\n");
        l293d_step_forward(&stepper2, delay_us, num_steps);
    }
    else if(sensor_values[3] < TENSION_UPPER_LIMIT)
    {
        // Move stepper 3
        // printf("Move 7: Stepper 2 stalled\n");
        l293d_step_forward(&stepper3, delay_us, num_steps);
    }
    else{
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
        l293d_step_backward(&stepper0, REVERSE_DELAY, 1);
    }

    l293d_stop(&stepper0);

    // Move stepper 2
    if(sensor_values[2] < TENSION_UPPER_LIMIT)
    {
        // Move stepper 2
        l293d_step_forward(&stepper2, delay_us, num_steps);
    }
    else{
        // printf("Move 8: Stalled\n");
    }
}

void move(int num_steps, int delay_us, int x, int y)
{
    // Combine x and y into a single move code
    int move_code = (x + 1) * 3 + (y + 1);

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

    switch (move_code)
    {
    case 0:
        // Move to -X, -Y
        move_0(num_steps, delay_us);
        break;
    case 1:
        // Move to -X, 0
        move_1(num_steps, delay_us);
        break;
    case 2:
        // Move to -X, +Y
        move_2(num_steps, delay_us);
        break;
    case 3:
        // Move to 0, -Y
        move_3(num_steps, delay_us);
        break;
    case 4:
        // Move to 0, 0
        move_4(num_steps, delay_us);
        break;
    case 5:
        // Move to 0, +Y
        move_5(num_steps, delay_us);
        break;
    case 6:
        // Move to +X, -Y
        move_6(num_steps, delay_us);
        break;
    case 7:
        // Move to +X, 0
        move_7(num_steps, delay_us);
        break;
    case 8:
        // Move to +X, +Y
        move_8(num_steps, delay_us);
        break;
    default:
        // Invalid move code
        printf("Invalid move code\n");
        break;
    }
}


void homing()
{
    // Home to +Y
    // Pull steppers 1 and 3 (step forward) until hard limit is reached.
    // Each time stepper 1 is pulled (step forward), stepper 0 is released (step backward)
    // Each time stepper 3 is pulled (step forward), stepper 2 is released (step backward)
    // Pull steppers 0 and 2 (step forward) independently until hard limit is reached.
    printf("Homing to +Y\n");

    int stepper_delay = 1000;

    while (mutex)
    {
        printf("Waiting for mutex to be released\n");
    };

    mutex = true;

    int stepper0_tension = ads1115_read_diff_0_1(&adc1);
    int stepper1_tension = ads1115_read_diff_2_3(&adc1);
    int stepper2_tension = ads1115_read_diff_0_1(&adc2);
    int stepper3_tension = ads1115_read_diff_2_3(&adc2);

    // Loosen stepper 0 to lower limit
    while (stepper0_tension > TENSION_LOWER_LIMIT)
    {
        l293d_step_backward(&stepper0, stepper_delay, 1);
        stepper0_tension = ads1115_read_diff_0_1(&adc1);
    }

    // Tighten stepper 1 to hard upper limit
    while (stepper1_tension < TENSION_HARD_UPPER_LIMIT)
    {
        l293d_step_forward(&stepper1, stepper_delay, 1);
        stepper1_tension = ads1115_read_diff_2_3(&adc1);

        // Maintain tension of stepper 0 to lower limit
        stepper0_tension = ads1115_read_diff_0_1(&adc1);
        while (stepper0_tension > TENSION_LOWER_LIMIT)
        {
            l293d_step_backward(&stepper0, stepper_delay, 1);
            stepper0_tension = ads1115_read_diff_0_1(&adc1);
        }
    }

    // Loosen stepper 3 to lower limit
    while (stepper3_tension > TENSION_LOWER_LIMIT)
    {
        l293d_step_backward(&stepper3, stepper_delay, 1);
        stepper3_tension = ads1115_read_diff_2_3(&adc2);
    }

    // Tighten stepper 2 to hard upper limit
    while (stepper2_tension < TENSION_HARD_UPPER_LIMIT)
    {
        l293d_step_forward(&stepper2, stepper_delay, 1);
        stepper2_tension = ads1115_read_diff_0_1(&adc2);

        // Maintain tension of stepper 3 to lower limit
        stepper3_tension = ads1115_read_diff_2_3(&adc2);
        while (stepper3_tension > TENSION_LOWER_LIMIT)
        {
            l293d_step_backward(&stepper3, 5000, 1);
            stepper3_tension = ads1115_read_diff_2_3(&adc2);
        }
    }

    // Home to -Y
    // Pull steppers 0 and 3 (step forward) until hard limit is reached.
    // Each time stepper 0 is pulled (step forward), stepper 1 is released (step backward)
    // Each time stepper 3 is pulled (step forward), stepper 2 is released (step backward)
    // Pull steppers 1 and 2 (step forward) independently until hard limit is reached.

    // Loosen stepper 1 to lower limit
    while (stepper1_tension > TENSION_LOWER_LIMIT)
    {
        l293d_step_backward(&stepper1, stepper_delay, 1);
        stepper1_tension = ads1115_read_diff_2_3(&adc1);
    }

    // Tighten stepper 0 to hard upper limit
    while (stepper0_tension < TENSION_HARD_UPPER_LIMIT)
    {
        l293d_step_forward(&stepper0, stepper_delay, 1);
        stepper0_tension = ads1115_read_diff_0_1(&adc1);

        // Maintain tension of stepper 1 to lower limit
        stepper1_tension = ads1115_read_diff_2_3(&adc1);
        while (stepper1_tension > TENSION_LOWER_LIMIT)
        {
            l293d_step_backward(&stepper1, stepper_delay, 1);
            stepper1_tension = ads1115_read_diff_2_3(&adc1);
        }
    }

    // Loosen stepper 2 to lower limit
    while (stepper2_tension > TENSION_LOWER_LIMIT)
    {
        l293d_step_backward(&stepper2, stepper_delay, 1);
        stepper2_tension = ads1115_read_diff_0_1(&adc2);
    }

    // Tighten stepper 3 to hard upper limit
    while (stepper3_tension < TENSION_HARD_UPPER_LIMIT)
    {
        l293d_step_forward(&stepper3, stepper_delay, 1);
        stepper3_tension = ads1115_read_diff_2_3(&adc2);

        // Maintain tension of stepper 2 to lower limit
        stepper2_tension = ads1115_read_diff_0_1(&adc2);
        while (stepper2_tension > TENSION_LOWER_LIMIT)
        {
            l293d_step_backward(&stepper2, stepper_delay, 1);
            stepper2_tension = ads1115_read_diff_0_1(&adc2);
        }
    }

    mutex = false;
}

int main()
    {
    stdio_init_all();

    setup_steppers();

    i2c_init(i2c_default, 400 * 1000); // 400kHz I2C
    gpio_set_function(I2C_SDA, GPIO_FUNC_I2C);
    gpio_set_function(I2C_SCL, GPIO_FUNC_I2C);
    gpio_pull_up(I2C_SDA);
    gpio_pull_up(I2C_SCL);

    // Initialize Onboard LED
    gpio_init(ONBOARD_LED);
    gpio_set_dir(ONBOARD_LED, GPIO_OUT);

    ads1115_init(&adc1, I2C_PORT, 0x48);
    ads1115_init(&adc2, I2C_PORT, 0x49);

    calibrate();

    int magnitude = 4;

    printf("Homing...\n");
    // homing();
    printf("Homing complete\n");

    multicore_launch_core1(read_ads111_task);

    while (true)
    {
        // printf("Hello, world!\n");

        // if (stop_flag)
        // {
        //     printf("Stopping program\n");
        //     break;
        // }

        // Wait for mutex to be released
        // while (mutex)
        // {
        //     printf("Waiting for mutex to be released\n");
        // };

        // // Set mutex
        // mutex = true;

        // // Set the control vector for the stepper controller
        // for (int i = 0; i < NUM_STEPPERS; i++)
        // {
        //     if (sensor_values[i] > TENSION_UPPER_LIMIT)
        //     {
        //         vector[i] = -1;
        //     }
        //     else if (sensor_values[i] < TENSION_LOWER_LIMIT)
        //     {
        //         vector[i] = 1;
        //     }
        //     else
        //     {
        //         vector[i] = 0;
        //     }
        // }

        // // Release mutex
        // mutex = false;

        // printf("Load[%d, %d, %d, %d]\n", sensor_values[0], sensor_values[1], sensor_values[2], sensor_values[3]);


        // printf("Vector: [%d, %d, %d, %d]\n", vector[0], vector[1], vector[2], vector[3]);

        // // Move the steppers
        // l293d_controller_vector(&controller, 5000, vector);

        // move(1, 10000, 0, -1);

        // sleep_ms(10);

        int delay = 4;

        // Move 0, +Y
        for(int i = 0; i < 1000; i++)
        {
            move(1, FORWARD_DELAY, 0, 1);
            sleep_ms(delay);
        }

        // Move 0, -Y
        for(int i = 0; i < 1000; i++)
        {
            move(1, FORWARD_DELAY, 0, -1);
            sleep_ms(delay);
        }

        // Move +X, 0
        for(int i = 0; i < FORWARD_DELAY; i++)
        {
            move(1, FORWARD_DELAY, 1, 0);
            sleep_ms(delay);
        }

        // Move -X, 0
        for(int i = 0; i < 1000; i++)
        {
            move(1, FORWARD_DELAY, -1, 0);
            sleep_ms(delay);
        }
    }

    return 0;
}
