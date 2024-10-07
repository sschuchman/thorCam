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
#define STEPPER0_PIN2 11
#define STEPPER0_PIN3 12
#define STEPPER0_PIN4 13

#define STEPPER1_PIN1 14
#define STEPPER1_PIN2 15
#define STEPPER1_PIN3 16
#define STEPPER1_PIN4 17

#define STEPPER2_PIN1 18
#define STEPPER2_PIN2 19
#define STEPPER2_PIN3 20
#define STEPPER2_PIN4 21

#define STEPPER3_PIN1 22
#define STEPPER3_PIN2 26
#define STEPPER3_PIN3 27
#define STEPPER3_PIN4 28

#define NUM_STEPPERS 4

#define I2C_PORT i2c0
#define I2C_SCL 5
#define I2C_SDA 4

l293d_t stepper0, stepper1, stepper2, stepper3;
l293d_controller_t controller;

ads1115_t adc1, adc2;
int16_t diff_0_1_adc1, diff_2_3_adc1, diff_0_1_adc2, diff_2_3_adc2;

#define TENSION_HARD_UPPER_LIMIT 300
#define TENSION_UPPER_LIMIT 60
// #define TENSION_OPTIMAL 90
#define TENSION_LOWER_LIMIT 20

#define false 0
#define true 1

volatile bool mutex = false;
volatile bool stop_flag = false;
int16_t diff_array[4] = {0, 0, 0, 0};

int8_t vector[] = {1, 1, 1, 1};

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

        // Update diff_array
        diff_array[0] = diff_0_1_adc1;
        diff_array[1] = diff_2_3_adc1;
        diff_array[2] = diff_0_1_adc2;
        diff_array[3] = diff_2_3_adc2;

        // Release mutex
        mutex = false;

        sleep_ms(10);

        char command[100];

        if (fgets(command, sizeof(command), stdin) != NULL)
        {
            // Remove newline character if present
            command[strcspn(command, "\n")] = 0;
            if (strcmp(command, "stop") == 0)
            {
                stop_flag = true;
                break;
            }
        }
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

void move_y_positive(int num_steps, int delay, int steps)
{
    for (int i = 0; i < num_steps; i++)
    {

        // Read tension from steppers 1 and 2 to check they are not over tensioned
        while (mutex)
        {
            printf("Waiting for mutex to be released\n");
        };

        // Set mutex
        mutex = true;

        int diff_2_3 = ads1115_read_diff_2_3(&adc1);
        int diff_0_1 = ads1115_read_diff_0_1(&adc2);

        printf("diff_2_3: %d\n", diff_2_3);
        printf("diff_0_1: %d\n", diff_0_1);

        // Release mutex
        mutex = false;

        if (diff_2_3 > TENSION_HARD_UPPER_LIMIT || diff_0_1 > TENSION_HARD_UPPER_LIMIT)
        {
            printf("Steppers are over tensioned\n");
            return;
        }
        else
        {
            // Move the steppers
            printf("About to move...\n");
            l293d_step_forward(&stepper1, delay, 4);
            l293d_step_forward(&stepper2, delay, 4);

            l293d_step_backward(&stepper0, delay, 8);
            l293d_step_backward(&stepper3, delay, 8);
            printf("movement complete\n");
        }
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

    int stepper_delay = 10000;

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

    ads1115_init(&adc1, I2C_PORT, 0x48);
    ads1115_init(&adc2, I2C_PORT, 0x49);

    // Wait for serial command to start calibration
    char command[10];
    printf("Enter 'stop' at any time to stop the program\n");
    printf("Enter 'start' to begin calibration:\n");
    while (true)
    {
        if (fgets(command, sizeof(command), stdin) != NULL)
        {
            // Remove newline character if present
            command[strcspn(command, "\n")] = 0;
            if (strcmp(command, "start") == 0)
            {
                break;
            }
            printf("Invalid command. Enter 'start' to begin calibration:\n");
        }
    }

    calibrate();

    int num_steps = 200;
    int speed = 1;
    int delay = 0;

    //

    int magnitude = 4;

    // move_y_positive(20, 10000, 1000);

    sleep_ms(1000);

    printf("Homing...\n");
    // homing();
    printf("Homing complete\n");

    multicore_launch_core1(read_ads111_task);

    while (true)
    {
        // printf("Hello, world!\n");

        if (stop_flag)
        {
            printf("Stopping program\n");
            break;
        }

        // Wait for mutex to be released
        while (mutex)
        {
            printf("Waiting for mutex to be released\n");
        };

        // Set mutex
        mutex = true;

        // Set the control vector for the stepper controller
        for (int i = 0; i < NUM_STEPPERS; i++)
        {
            if (diff_array[i] > TENSION_UPPER_LIMIT)
            {
                vector[i] = -1;
            }
            else if (diff_array[i] < TENSION_LOWER_LIMIT)
            {
                vector[i] = 1;
            }
            else
            {
                vector[i] = 0;
            }
        }

        // Release mutex
        mutex = false;

        printf("Vector: %d, %d, %d, %d\n", vector[0], vector[1], vector[2], vector[3]);

        l293d_controller_vector(&controller, 100, vector);

        sleep_ms(100);
    }

    return 0;
}
