#include "pico/stdlib.h"
// #include "hx711.h"
#include "l293d.h"
#include <stdio.h>

#define LED_PIN 25 // Onboard LED pin
#define LOAD_1_DOUT 21 // GPIO pin for HX711 DOUT
#define LOAD_1_SCK 22  // GPIO pin for HX711 SCK
#define LOAD_2_DOUT 26 // GPIO pin for HX711 DOUT
#define LOAD_2_SCK 27  // GPIO pin for HX711 SCK


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

int main() {
    stdio_init_all();

    // Initialize the LED pin
    gpio_init(LED_PIN);
    gpio_set_dir(LED_PIN, GPIO_OUT);

    // // Initialize the HX711
    // hx711_t load1;
    // hx711_init(&load1, LOAD_1_DOUT, LOAD_1_SCK, "Load 1");
    // hx711_tare(&load1);

    // hx711_t load2;
    // hx711_init(&load2, LOAD_2_DOUT, LOAD_2_SCK, "Load 2");
    // hx711_tare(&load2);

    // // Verify the tare value
    // int32_t tare_value = hx711_read(&load1);
    // printf("Tare Value: %d\n", tare_value);

    // int32_t tare_value2 = hx711_read(&load2);
    // printf("Tare Value 2: %d\n", tare_value2);

    // Blink the LED to debug before starting the scheduler
    // for (int i = 300; i > 0; i -= 20) {
    //     gpio_put(LED_PIN, 1);
    //     sleep_ms(i);
    //     gpio_put(LED_PIN, 0);
    //     sleep_ms(i);
    // }

    l293d_t stepper0, stepper1, stepper2, stepper3;
    l293d_t steppers [] = {stepper0, stepper1, stepper2, stepper3};
    l293d_init(&stepper0, STEPPER0_PIN1, STEPPER0_PIN2, STEPPER0_PIN3, STEPPER0_PIN4);
    l293d_init(&stepper1, STEPPER1_PIN1, STEPPER1_PIN2, STEPPER1_PIN3, STEPPER1_PIN4);
    l293d_init(&stepper2, STEPPER2_PIN1, STEPPER2_PIN2, STEPPER2_PIN3, STEPPER2_PIN4);
    l293d_init(&stepper3, STEPPER3_PIN1, STEPPER3_PIN2, STEPPER3_PIN3, STEPPER3_PIN4);

    // Main loop
    while (true) {
        // Read data from HX711
        // int32_t value = hx711_read(&load1);

        // printf("HX711 Value: %d\n", value);

        for(int i=0; i<4; i++){
            l293d_step_forward(&steppers[i], 1000, 8);
            l293d_step_backward(&steppers[i], 1000, 8);
        }


        // Blink the LED (optional)
        // gpio_put(LED_PIN, 1);
        // sleep_ms(500);
        // gpio_put(LED_PIN, 0);
        // sleep_ms(500);
    }

    return 0;
}