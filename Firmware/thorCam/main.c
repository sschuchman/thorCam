#include "pico/stdlib.h"
#include "include/l293d.h"
#include <stdio.h>
#include "include/ads1115.h"
#include "hardware/i2c.h"
#include "pico/multicore.h"
#include "include/movement.h"
#include "include/sensor.h"

#define LED_PIN 25 // Onboard LED pin

#define I2C_PORT i2c0
#define I2C_SCL 5
#define I2C_SDA 4

#define UART_TX_PIN 0
#define UART_RX_PIN 1
#define BAUD_RATE 115200
#define UART_ID uart0
#define BUFFER_SIZE 256

#define false 0
#define true 1

void uart_rx_task()
{
    char rx_buffer[BUFFER_SIZE];
    int buffer_index = 0;

    while (true)
    {
        // Check if there is data available to read
        if (uart_is_readable(UART_ID))
        {
            // Read a character from the UART
            char ch = uart_getc(UART_ID);
            // Echo the received character back to the UART
            uart_putc(UART_ID, ch);

            // Check for newline character
            if (ch == '\n')
            {
                // Null-terminate the string
                rx_buffer[buffer_index] = '\0';
                // Print the received string to the console
                printf("Received string: %s\n", rx_buffer);

                // Parse the received string
                int x, y;
                if (sscanf(rx_buffer, "Move[%d,%d]", &x, &y) == 2)
                {
                    // Check if x and y are valid values (+1, -1, or 0)
                    if ((x == 1 || x == -1 || x == 0) && (y == 1 || y == -1 || y == 0))
                    {
                        // Call the move function with the parsed values
                        // move(x, y);
                        printf("Moving with x = %d, y = %d\n", x, y);
                        move(100, 3000, x, y);
                    }
                    else
                    {
                        printf("Invalid values for x or y\n");
                    }
                }
                else
                {
                    printf("Invalid command format\n");
                }

                // Reset the buffer index for the next command
                buffer_index = 0;
            }
            else
            {
                // Store the character in the buffer
                if (buffer_index < BUFFER_SIZE - 1)
                {
                    rx_buffer[buffer_index++] = ch;
                }
                else
                {
                    // Buffer overflow, reset the buffer index
                    buffer_index = 0;
                }
            }
        }
    }
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

    calibrate();

    multicore_launch_core1(read_ads111_task);

    uart_init(UART_ID, BAUD_RATE);
    gpio_set_function(UART_TX_PIN, GPIO_FUNC_UART);
    gpio_set_function(UART_RX_PIN, GPIO_FUNC_UART);
    uart_set_irq_enables(UART_ID, true, false);

    //uart_rx_task();

    while (true)
    {
        int delay = 4;

        // Move 0, +Y
        for (int i = 0; i < 1000; i++)
        {
            move(1, FORWARD_DELAY, 0, 1);
            sleep_ms(delay);
        }

        // Move 0, -Y
        for (int i = 0; i < 1000; i++)
        {
            move(1, FORWARD_DELAY, 0, -1);
            sleep_ms(delay);
        }

        // Move +X, 0
        for (int i = 0; i < FORWARD_DELAY; i++)
        {
            move(1, FORWARD_DELAY, 1, 0);
            sleep_ms(delay);
        }

        // Move -X, 0
        for (int i = 0; i < 1000; i++)
        {
            move(1, FORWARD_DELAY, -1, 0);
            sleep_ms(delay);
        }
    }

    return 0;
}
