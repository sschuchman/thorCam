#include "pico/stdlib.h"
#include "include/l293d.h"
#include <stdio.h>
#include "include/ads1115.h"
#include "hardware/i2c.h"
#include "pico/multicore.h"
#include "include/movement.h"
#include "include/sensor.h"
#include "include/uart_rx.h"

#define I2C_PORT i2c0
#define I2C_SCL 5
#define I2C_SDA 4

#define false 0
#define true 1

void setup(){
    stdio_init_all();

    setup_steppers();

    i2c_init(i2c_default, 400 * 1000); // 400kHz I2C
    gpio_set_function(I2C_SDA, GPIO_FUNC_I2C);
    gpio_set_function(I2C_SCL, GPIO_FUNC_I2C);
    gpio_pull_up(I2C_SDA);
    gpio_pull_up(I2C_SCL);

    ads1115_init(&adc1, I2C_PORT, 0x48);
    ads1115_init(&adc2, I2C_PORT, 0x49);

    uart_init(UART_ID, BAUD_RATE);
    gpio_set_function(UART_TX_PIN, GPIO_FUNC_UART);
    gpio_set_function(UART_RX_PIN, GPIO_FUNC_UART);
    uart_set_irq_enables(UART_ID, true, false);
}

void core1_task(){
    while(true){
        read_ads111_task();
        
        sleep_ms(10);
    }
}


int main()
{
    setup();

    calibrate();

    multicore_launch_core1(core1_task);

    while (true)
    {
        uart_rx_task();

        // for(int i=0; i<1000; i++){
        //     move(1, 1000, 0, 1);
        //     sleep_ms(10);
        // }

        // for(int i=0; i<1000; i++){
        //     move(1, 1000, 0, -1);
        //     sleep_ms(10);
        // }
    }

    return 0;
}
