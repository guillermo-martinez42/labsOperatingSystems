#ifndef OS_BBB_H
#define OS_BBB_H

/*
 * os_bbb.h - Hardware interface layer for BeagleBone Black (AM335x)
 *
 * Ported from os.h (QEMU VersatilePB).
 * The public API is identical; only the implementation in os_bbb.c
 * changes to target the AM335x UART0 (16550-compatible, 0x44E09000).
 */

/* Initialise UART0 at 115200 8N1 (must call before any UART I/O) */
void uart0_init(void);

/* Send a single character via UART0 */
void uart_putc(char c);

/* Receive a single character via UART0 (blocking) */
char uart_getc(void);

/* Send a null-terminated string via UART0 */
void uart_puts(const char *s);

/* Read one line of input via UART0 (with echo, CR/LF terminates) */
void uart_gets_input(char *buffer, int max_length);

#endif /* OS_BBB_H */
