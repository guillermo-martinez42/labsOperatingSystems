/*
 * main_bbb.c - Calculator application for BeagleBone Black
 *
 * Ported from lab02/Files/main.c.
 * The only change is:
 *   - #include "stdio.h" → #include "stdio_bbb.h"
 *   - uart0_init() is called first to bring up UART0 on the AM335x.
 *     (In the QEMU version UART was pre-initialised by the machine model.)
 */

#include "stdio_bbb.h"
#include "os_bbb.h"

void main(void)
{
    /* Initialise AM335x UART0 (clock + baud rate).
     * This step replaces QEMU's automatic UART setup. */
    uart0_init();

    PRINT("Program: Add Two Numbers\n");

    while (1) {
        /* --- Integer addition --- */
        int num1, num2;

        PRINT("Enter first integer: ");
        READ("%d", &num1);

        PRINT("Enter second integer: ");
        READ("%d", &num2);

        PRINT("Sum: %d\n", num1 + num2);

        /* --- Floating-point addition --- */
        float f1, f2;

        PRINT("Enter first float: ");
        READ("%f", &f1);

        PRINT("Enter second float: ");
        READ("%f", &f2);

        PRINT("Sum: %f\n", (double)(f1 + f2));
    }
}
