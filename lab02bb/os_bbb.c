/*
 * os_bbb.c - UART hardware driver for BeagleBone Black (AM335x Cortex-A8)
 *
 * Ported from os.c (QEMU VersatilePB / PL011).
 *
 * === Register-level changes (QEMU → BBB) ===
 *
 * | Constant/Field         | QEMU (PL011)            | BBB (AM335x 16550)       |
 * |------------------------|-------------------------|--------------------------|
 * | UART0_BASE             | 0x101f1000              | 0x44E09000               |
 * | TX data register       | DR  offset 0x00         | THR offset 0x00 (write)  |
 * | RX data register       | DR  offset 0x00 (read)  | RHR offset 0x00 (read)   |
 * | Status register        | FR  offset 0x18         | LSR offset 0x14          |
 * | TX-busy flag           | FR[5] TXFF (1=full)     | LSR[5] THRE (1=empty/ok) |
 * | RX-empty flag          | FR[4] RXFE (1=empty)    | LSR[0] DR   (1=data rdy) |
 * | Clock enable           | not needed in QEMU      | CM_WKUP_UART0_CLKCTRL    |
 * | Mode / baud setup      | pre-configured by QEMU  | MDR1, LCR, DLL, DLH      |
 *
 * Baud rate: 115 200 bps, 8N1.
 * Functional clock: 48 MHz (DPLL_PER output).
 * Divisor: 48 000 000 / (16 × 115 200) = 26 (0x1A).
 */

#include "os_bbb.h"

/* ------------------------------------------------------------------ */
/*  AM335x UART0 base address (TRM Table 2-1)                          */
/* ------------------------------------------------------------------ */
#define UART0_BASE              0x44E09000UL

/* UART register offsets — operational mode (LCR[7] = 0) */
#define UART_THR                0x00U   /* Transmit Holding Register (write) */
#define UART_RHR                0x00U   /* Receive  Holding Register (read)  */
#define UART_IER                0x04U   /* Interrupt Enable Register         */
#define UART_FCR                0x08U   /* FIFO Control Register (write)     */
#define UART_LCR                0x0CU   /* Line Control Register             */
#define UART_LSR                0x14U   /* Line Status Register              */
#define UART_MDR1               0x20U   /* Mode Definition Register 1        */

/* UART register offsets — config mode (LCR[7] = 1) */
#define UART_DLL                0x00U   /* Divisor Latch Low                 */
#define UART_DLH                0x04U   /* Divisor Latch High                */

/* LSR bit masks */
#define UART_LSR_THRE           (1U << 5)   /* TX holding register empty     */
#define UART_LSR_DR             (1U << 0)   /* RX data ready                 */

/* ------------------------------------------------------------------ */
/*  AM335x Clock Module – Wakeup Domain (CM_WKUP)                      */
/*  TRM Section 8.1.12, Table 8-177                                    */
/* ------------------------------------------------------------------ */
#define CM_WKUP_BASE            0x44E00400UL
#define CM_WKUP_UART0_CLKCTRL  0xB4U       /* offset within CM_WKUP        */
#define CLKCTRL_MODULEMODE_EN  0x2U        /* MODULEMODE = ENABLE           */
#define CLKCTRL_IDLEST_MASK    (3U << 16)  /* IDLEST bits (0 = functional)  */

/* ------------------------------------------------------------------ */
/*  Register access helper                                              */
/* ------------------------------------------------------------------ */
#define REG32(base, off) \
    (*((volatile unsigned int *)((unsigned int)(base) + (unsigned int)(off))))

/* ------------------------------------------------------------------ */
/*  UART0 initialisation                                               */
/* ------------------------------------------------------------------ */
void uart0_init(void)
{
    /* 1. Gate the UART0 functional clock on via CM_WKUP */
    REG32(CM_WKUP_BASE, CM_WKUP_UART0_CLKCTRL) = CLKCTRL_MODULEMODE_EN;
    /* Wait until the module leaves idle (IDLEST field → 0b00) */
    while (REG32(CM_WKUP_BASE, CM_WKUP_UART0_CLKCTRL) & CLKCTRL_IDLEST_MASK);

    /* 2. Disable UART (MDR1 = 0x7) before touching baud-rate registers */
    REG32(UART0_BASE, UART_MDR1) = 0x7U;

    /* 3. Enter config mode A (LCR[7] = 1) to access DLL/DLH
     *    0x83 = 1000_0011b  → DLAB=1, 8 data bits, no parity, 1 stop bit */
    REG32(UART0_BASE, UART_LCR) = 0x83U;

    /* 4. Set divisor for 115200 baud @ 48 MHz clock */
    REG32(UART0_BASE, UART_DLL) = 26U;     /* 0x1A */
    REG32(UART0_BASE, UART_DLH) = 0U;

    /* 5. Return to operational mode: 8N1, DLAB = 0
     *    0x03 = 0000_0011b  → 8 data bits, no parity, 1 stop bit */
    REG32(UART0_BASE, UART_LCR) = 0x03U;

    /* 6. Mask all UART interrupts (bare-metal polling mode) */
    REG32(UART0_BASE, UART_IER) = 0x0U;

    /* 7. Enable TX/RX FIFOs and clear them (FCR = 0x07) */
    REG32(UART0_BASE, UART_FCR) = 0x07U;

    /* 8. Enable UART in UART 16× over-sampling mode (MDR1 = 0x0) */
    REG32(UART0_BASE, UART_MDR1) = 0x0U;
}

/* ------------------------------------------------------------------ */
/*  Send a single character                                            */
/* ------------------------------------------------------------------ */
void uart_putc(char c)
{
    /* Block until TX holding register is empty (LSR[5] = 1) */
    while (!(REG32(UART0_BASE, UART_LSR) & UART_LSR_THRE));
    REG32(UART0_BASE, UART_THR) = (unsigned int)(unsigned char)c;
}

/* ------------------------------------------------------------------ */
/*  Receive a single character (blocking)                              */
/* ------------------------------------------------------------------ */
char uart_getc(void)
{
    /* Block until RX data is ready (LSR[0] = 1) */
    while (!(REG32(UART0_BASE, UART_LSR) & UART_LSR_DR));
    return (char)(REG32(UART0_BASE, UART_RHR) & 0xFFU);
}

/* ------------------------------------------------------------------ */
/*  Send a null-terminated string                                      */
/* ------------------------------------------------------------------ */
void uart_puts(const char *s)
{
    while (*s) {
        uart_putc(*s++);
    }
}

/* ------------------------------------------------------------------ */
/*  Read one line of input (with local echo), CR or LF terminates     */
/* ------------------------------------------------------------------ */
void uart_gets_input(char *buffer, int max_length)
{
    int i = 0;
    char c;
    while (i < max_length - 1) {
        c = uart_getc();
        if (c == '\n' || c == '\r') {
            uart_putc('\n');
            break;
        }
        uart_putc(c);           /* local echo */
        buffer[i++] = c;
    }
    buffer[i] = '\0';
}
