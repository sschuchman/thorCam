#include "uart_rx.h"

void uart_rx_task()
{
    char rx_buffer[BUFFER_SIZE];
    int buffer_index = 0;

    // while (true)
    // {
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
        // }
    }
}
