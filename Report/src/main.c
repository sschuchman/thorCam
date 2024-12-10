/******************************************************************************
* Copyright (C) 2023 Advanced Micro Devices, Inc. All Rights Reserved.
* SPDX-License-Identifier: MIT
******************************************************************************/
/*
 * helloworld.c: simple test application
 *
 * This application configures UART 16550 to baud rate 9600.
 * PS7 UART (Zynq) is not initialized by this application, since
 * bootrom/bsp configures it to baud rate 115200
 *
 * ------------------------------------------------
 * | UART TYPE   BAUD RATE                        |
 * ------------------------------------------------
 *   uartns550   9600
 *   uartlite    Configurable only in HW design
 *   ps7_uart    115200 (configured by bootrom/bsp)
 */

#include <stdio.h>
#include "platform.h"
#include "xil_printf.h"

int main()
{
    init_platform();

    xil_printf("Fibonacci Sequence:\n\r");

    int n1 = 0, n2 = 1, n3, i;
    int terms = 5; // Number of terms to print

    xil_printf("%d\n\r", n1);
    xil_printf("%d\n\r", n2);

    for (i = 2; i < terms; i++) {
        n3 = n1 + n2;
        xil_printf("%d\n\r", n3);
        n1 = n2;
        n2 = n3;
    }

    cleanup_platform();
    return 0;
}
