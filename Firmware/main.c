#include "pico/stdlib.h"
#include "l293d.h"
#include <stdio.h>
#include "hx711.h"
#include "hardware/i2c.h"
#include "pico/multicore.h"
#include "shift_register.h"

#define LED_PIN 25 // Onboard LED pin
// #define LOAD_1_DOUT 21 // GPIO pin for HX711 DOUT
// #define LOAD_1_SCK 22  // GPIO pin for HX711 SCK
// #define LOAD_2_DOUT 26 // GPIO pin for HX711 DOUT
// #define LOAD_2_SCK 27  // GPIO pin for HX711 SCK


#define STEPPER0_PIN1 6
#define STEPPER0_PIN2 7
#define STEPPER0_PIN3 8
#define STEPPER0_PIN4 9

#define STEPPER1_PIN1 10
#define STEPPER1_PIN2 11
#define STEPPER1_PIN3 12
#define STEPPER1_PIN4 13

#define STEPPER2_PIN1 14
#define STEPPER2_PIN2 15
#define STEPPER2_PIN3 16
#define STEPPER2_PIN4 17

#define STEPPER3_PIN1 18
#define STEPPER3_PIN2 19
#define STEPPER3_PIN3 20
#define STEPPER3_PIN4 21

#define HX711_0_DOUT 2
#define HX711_1_DOUT 3
#define HX711_2_DOUT 26
#define HX711_3_DOUT 27
#define HX711_SCK 22

#define NUM_STEPPERS 4

hx711_t hx0, hx1, hx2, hx3;
l293d_t steppers[NUM_STEPPERS];
volatile int32_t weight_a = 0;

#define UPPER_THRESHOLD 150000
#define LOWER_THRESHOLD 100000

void setup_steppers() {
    // Initialize the steppers
    l293d_init(&steppers[0], STEPPER0_PIN1, STEPPER0_PIN2, STEPPER0_PIN3, STEPPER0_PIN4);
    l293d_init(&steppers[1], STEPPER1_PIN1, STEPPER1_PIN2, STEPPER1_PIN3, STEPPER1_PIN4);
    l293d_init(&steppers[2], STEPPER2_PIN1, STEPPER2_PIN2, STEPPER2_PIN3, STEPPER2_PIN4);
    l293d_init(&steppers[3], STEPPER3_PIN1, STEPPER3_PIN2, STEPPER3_PIN3, STEPPER3_PIN4);
}

void read_hx711_task() {
    while (true) {
        uint32_t start_time = time_us_32();

        int32_t raw_value_a = hx711_read_raw(&hx0);
        weight_a = raw_value_a - hx0.offset; // Adjust the raw value by the offset
        uint32_t end_time = time_us_32();
        uint32_t duration_a = end_time - start_time;

        printf("Channel A - Raw Value: %d, Weight: %d, Time: %u us\n", raw_value_a, weight_a, duration_a);
    }
}

void move_steppers_task() {
    while (true) {
        if (weight_a > UPPER_THRESHOLD) {
            for (int i = 0; i < NUM_STEPPERS; i++) {
                l293d_step_forward(&steppers[i], 1000, 1);
            }
        } else if(weight_a < LOWER_THRESHOLD) {
            for (int i = 0; i < NUM_STEPPERS; i++) {
                l293d_step_backward(&steppers[i], 1000, 1);
            }
        }
    }
}

void calibrate_hx711() {
    // Calibrate 0
    // Release tension
    l293d_step_backward(&steppers[0], 1000, 64);

    printf("Ensure the tension wire is properly aligned within the guide wheels...\n");

    // Tare the HX711
    printf("Taring... Please wait...\n");
    hx711_tare(&hx0, 20);
    printf("Tare complete\n");

    int32_t read_value = 0;
    int32_t adjusted_value = 0;

    while (adjusted_value < LOWER_THRESHOLD) {
        read_value = hx711_read_raw(&hx0);
        adjusted_value = read_value - hx0.offset;
        printf("Current tension: %d\n", adjusted_value);

        if (adjusted_value >= LOWER_THRESHOLD) {
            printf("Target wire tension reached\n");
            break;
        } else {
            printf("Tension is too low. Adjusting...\n");
            l293d_step_forward(&steppers[0], 10000, 4);
        }
        //sleep_ms(1000);
    }

    // Calibrate 1
    // Release tension
    l293d_step_backward(&steppers[1], 1000, 64);

    printf("Ensure the tension wire is properly aligned within the guide wheels...\n");

    // Tare the HX711
    printf("Taring... Please wait...\n");
    hx711_tare(&hx1, 20);
    printf("Tare complete\n");

    while (true) {
        read_value = hx711_read_raw(&hx1);
        adjusted_value = read_value - hx1.offset;
        printf("Current tension: %d\n", adjusted_value);

        if (adjusted_value >= UPPER_THRESHOLD) {
            printf("Target wire tension reached\n");
            break;
        } else {
            printf("Tension is too low. Adjusting...\n");
            l293d_step_forward(&steppers[1], 10000, 4);
        }
    }

    // Calibrate 2
    // Release tension
    l293d_step_backward(&steppers[2], 1000, 64);

    printf("Ensure the tension wire is properly aligned within the guide wheels...\n");

    // Tare the HX711
    printf("Taring... Please wait...\n");
    hx711_tare(&hx2, 20);
    printf("Tare complete\n");

    while (true) {
        read_value = hx711_read_raw(&hx2);
        adjusted_value = read_value - hx2.offset;
        printf("Current tension: %d\n", adjusted_value);

        if (adjusted_value >= LOWER_THRESHOLD) {
            printf("Target wire tension reached\n");
            break;
        } else {
            printf("Tension is too low. Adjusting...\n");
            l293d_step_forward(&steppers[2], 10000, 4);
        }
    }

    // Calibrate 3
    // Release tension
    l293d_step_backward(&steppers[3], 1000, 64);

    printf("Ensure the tension wire is properly aligned within the guide wheels...\n");

    // Tare the HX711
    printf("Taring... Please wait...\n");
    hx711_tare(&hx3, 20);
    printf("Tare complete\n");

    while (true) {
        read_value = hx711_read_raw(&hx3);
        adjusted_value = read_value - hx3.offset;
        printf("Current tension: %d\n", adjusted_value);

        if (adjusted_value >= UPPER_THRESHOLD) {
            printf("Target wire tension reached\n");
            break;
        } else {
            printf("Tension is too low. Adjusting...\n");
            l293d_step_forward(&steppers[3], 10000, 4);
        }
    }
}


int main() {
    stdio_init_all();

    // Manually init the HX711 SCK pin
    // gpio_init(HX711_SCK);
    // gpio_set_dir(HX711_SCK, GPIO_OUT);

    // hx711_init(&hx0, HX711_0_DOUT, HX711_SCK, "Load Cell 1");
    // hx711_set_gain(&hx0, 128);

    // hx711_init(&hx1, HX711_1_DOUT, HX711_SCK, "Load Cell 2");
    // hx711_set_gain(&hx1, 128);

    // hx711_init(&hx2, HX711_2_DOUT, HX711_SCK, "Load Cell 3");
    // hx711_set_gain(&hx2, 128);

    // hx711_init(&hx3, HX711_3_DOUT, HX711_SCK, "Load Cell 4");
    // hx711_set_gain(&hx3, 128);

    // setup_steppers();

    // calibrate_hx711();

    // multicore_launch_core1(read_hx711_task);

    // move_steppers_task();

    shift_register_init();

    shift_register_clear();
    shift_register_enable_output(true);

    



    while (true) {
        printf("Hello World\n");

        for (int i = 0; i < 16; i++) {
            shift_register_write_16(1 << i);
            sleep_ms(500);
        }
    }

    return 0;
}