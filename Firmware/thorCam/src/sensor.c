#include "sensor.h"

// Variable definitions
ads1115_t adc1, adc2;
int16_t diff_0_1_adc1, diff_2_3_adc1, diff_0_1_adc2, diff_2_3_adc2;
int16_t sensor_values[4] = {0, 0, 0, 0};

volatile bool mutex = false;
volatile bool stop_flag = false;

void read_ads111_task()
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
    // while (mutex)
    // {
    //     printf("Waiting for mutex to be released\n");
    // };

    // // Set mutex
    // mutex = true;

    gpio_put(LED_PIN, 1);
    sleep_ms(10);

    // Update diff_array
    sensor_values[0] = (diff_0_1_adc1 > 0) ? diff_0_1_adc1 : 0;
    sensor_values[1] = (diff_2_3_adc1 > 0) ? diff_2_3_adc1 : 0;
    sensor_values[2] = (diff_0_1_adc2 > 0) ? diff_0_1_adc2 : 0;
    sensor_values[3] = (diff_2_3_adc2 > 0) ? diff_2_3_adc2 : 0;
    // sensor_values[0] = diff_0_1_adc1;
    // sensor_values[1] = diff_2_3_adc1;
    // sensor_values[2] = diff_0_1_adc2;
    // sensor_values[3] = diff_2_3_adc2;

    // // Release mutex
    // mutex = false;

    gpio_put(LED_PIN, 0);

    // Send the load values to the controller
    printf("Load[%d, %d, %d, %d]\n", sensor_values[0], sensor_values[1], sensor_values[2], sensor_values[3]);
}

void calibrate()
{
    printf("Calibrating...\n");

    // initialize the onboard LED
    gpio_init(LED_PIN);

    // set the onboard LED as output
    gpio_set_dir(LED_PIN, GPIO_OUT);

    int unwinding_delay = 5000;
    int unwinding_steps = 200;

    l293d_stop(&stepper0);
    l293d_stop(&stepper1);
    l293d_stop(&stepper2);
    l293d_stop(&stepper3);

    printf("Calibrating 0\n");
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
        sleep_ms(1);
    }

    // Stop the stepper
    l293d_stop(&stepper0);

    printf("Calibrating 1\n");
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
        sleep_ms(1);
    }

    // Stop the stepper
    l293d_stop(&stepper1);

    printf("Calibrating 2\n");
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
        sleep_ms(1);
    }

    // Stop the stepper
    l293d_stop(&stepper2);

    printf("Calibrating 3\n");
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
        sleep_ms(1);
    }

    // Stop the stepper
    l293d_stop(&stepper3);

    printf("Calibration complete\n");
}
