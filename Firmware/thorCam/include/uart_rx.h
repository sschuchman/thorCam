#ifndef UART_RX_H
#define UART_RX_H

#include "pico/stdlib.h"
#include <stdio.h>
#include "include/movement.h"

#define UART_TX_PIN 0
#define UART_RX_PIN 1
#define BAUD_RATE 115200
#define UART_ID uart0
#define BUFFER_SIZE 256

void uart_rx_task();

#endif // UART_RX_H