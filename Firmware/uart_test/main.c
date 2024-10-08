#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/uart.h"

// Define UARTs and pins
#define UART0_ID uart0
#define UART0_TX_PIN 0
#define UART0_RX_PIN 1

#define UART1_ID uart1
#define UART1_TX_PIN 8
#define UART1_RX_PIN 9

int main()
{
    stdio_init_all();

    // Set up UART0 for debug probe
    uart_init(UART0_ID, 115200);
    gpio_set_function(UART0_TX_PIN, GPIO_FUNC_UART);
    gpio_set_function(UART0_RX_PIN, GPIO_FUNC_UART);

    // Set up UART1 for communication with Raspberry Pi
    uart_init(UART1_ID, 115200);
    gpio_set_function(UART1_TX_PIN, GPIO_FUNC_UART);
    gpio_set_function(UART1_RX_PIN, GPIO_FUNC_UART);

    // Send out a string via UART0 (debug probe)
    uart_puts(UART0_ID, "Hello, UART0!\n");

    // Send out a string via UART1 (Raspberry Pi)
    uart_puts(UART1_ID, "Hello, UART1!\n");

    while (true) {
        // Send debug message via UART0
        uart_puts(UART0_ID, "Debug: Hello, world!\n");

        // Send data to Raspberry Pi via UART1
        uart_puts(UART1_ID, "Data: Hello, Raspberry Pi!\n");

        sleep_ms(1000);
    }
}