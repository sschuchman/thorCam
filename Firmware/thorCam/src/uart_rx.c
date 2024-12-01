#include "uart_rx.h"
#include <string.h> // For strstr

void uart_rx_task()
{
    static char rx_buffer[BUFFER_SIZE] = {0}; // Buffer to store received characters
    static int buffer_index = 0;             // Current index in the buffer

    // Check if data is available to read
    if (uart_is_readable(UART_ID))
    {
        // Read a character from UART
        char ch = uart_getc(UART_ID);
        // printf("Received char: '%c' (ASCII: %d)\n", ch, ch); // Debug each character
        uart_putc(UART_ID, ch);                              // Echo the character back

        // Append the character to the buffer
        if (buffer_index < BUFFER_SIZE - 1)
        {
            rx_buffer[buffer_index++] = ch;
            rx_buffer[buffer_index] = '\0'; // Null-terminate after appending
            // printf("Updated rx_buffer: '%s'\n", rx_buffer); // Debug updated buffer
        }
        else
        {
            // Buffer overflow handling
            // printf("Buffer overflow. Resetting index.\n");
            buffer_index = 0;
            memset(rx_buffer, 0, sizeof(rx_buffer)); // Clear the buffer
        }

        // Check for newline (end of command)
        if (ch == '\n')
        {
            // printf("Debug raw rx_buffer: '%s'\n", rx_buffer);

            // Trim trailing whitespace (e.g., \n, \r)
            int i = buffer_index - 1;
            while (i >= 0 && (rx_buffer[i] == '\n' || rx_buffer[i] == '\r'))
            {
                rx_buffer[i] = '\0';
                i--;
            }

            // printf("Debug trimmed rx_buffer: '%s'\n", rx_buffer);

            // Look for the "Move" substring in the received command
            char *start_of_command = strstr(rx_buffer, "Move[");
            if (start_of_command != NULL)
            {
                int x, y;
                // Parse the values of x and y, ignoring trailing characters
                if (sscanf(start_of_command, "Move[%d,%d", &x, &y) == 2)
                {
                    printf("Parsed successfully: x = %d, y = %d\n", x, y);

                    // Check validity of x and y values
                    if ((x == 1 || x == -1 || x == 0) && (y == 1 || y == -1 || y == 0))
                    {
                        // Call the move function
                        // move(10, 3000, x, y);
                        move(15, 4000, x, y);
                    }
                    else
                    {
                        printf("Invalid values for x or y\n");
                    }
                }
                else
                {
                    printf("Found 'Move' but failed to parse values: '%s'\n", start_of_command);
                }
            }
            else
            {
                printf("No valid 'Move' command found in: '%s'\n", rx_buffer);
            }

            // Reset the buffer index for the next command
            buffer_index = 0;
            memset(rx_buffer, 0, sizeof(rx_buffer)); // Clear the buffer
        }
    }
}
