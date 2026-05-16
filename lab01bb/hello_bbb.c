/*
 * hello_bbb.c - "Hello World" for BeagleBone Black (AM335x Cortex-A8)
 *
 * Ported from the QEMU VersatilePB version (hello.c).
 *
 * === Register changes (QEMU VersatilePB → AM335x) ===
 *
 * | Item             | QEMU (PL011)         | BBB (16550)              |
 * |------------------|----------------------|--------------------------|
 * | UART0 base       | 0x101f1000           | 0x44E09000               |
 * | TX register      | DR  (offset 0x00)    | THR (offset 0x00, write) |
 * | Status register  | FR  (offset 0x18)    | LSR (offset 0x14)        |
 * | TX-ready bit     | FR[5] TXFF=0 (not full)| LSR[5] THRE=1 (empty) |
 * | Clock control    | none needed in QEMU  | CM_WKUP_UART0_CLKCTRL   |
 * | UART mode reg    | none                 | MDR1 (offset 0x20)       |
 *
 * The AM335x UART is a TI-enhanced 16550-compatible controller.
 * Baud rate = 115200, 8N1.  Functional clock = 48 MHz.
 * Divisor   = 48 000 000 / (16 × 115 200) = 26  (0x1A)
 */

/* ------------------------------------------------------------------ */
/*  Hardware addresses                                                  */
/* ------------------------------------------------------------------ */

/* AM335x UART0 base (TRM Table 2-1) */
#define UART0_BASE              0x44E09000UL

/* UART register offsets (16550 operational mode, LCR[7]=0) */
#define UART_THR                0x00U   /* Transmit Holding Register (write) */
#define UART_RHR                0x00U   /* Receive  Holding Register (read)  */
#define UART_IER                0x04U   /* Interrupt Enable Register         */
#define UART_FCR                0x08U   /* FIFO Control Register (write)     */
#define UART_LCR                0x0CU   /* Line Control Register             */
#define UART_LSR                0x14U   /* Line Status Register              */
#define UART_MDR1               0x20U   /* Mode Definition Register 1        */

/* UART register offsets (config mode: LCR[7]=1) */
#define UART_DLL                0x00U   /* Divisor Latch Low                 */
#define UART_DLH                0x04U   /* Divisor Latch High                */

/* LSR bit masks */
#define UART_LSR_THRE           (1U << 5)   /* TX holding reg empty → ready  */
#define UART_LSR_DR             (1U << 0)   /* Data ready (RX)               */

/* ------------------------------------------------------------------ */
/*  Clock Module (CM_WKUP) - required to gate UART0 functional clock   */
/*  AM335x TRM Section 8.1.12 / Table 8-177                           */
/* ------------------------------------------------------------------ */
#define CM_WKUP_BASE            0x44E00400UL
#define CM_WKUP_UART0_CLKCTRL  0xB4U   /* offset within CM_WKUP            */
#define CLKCTRL_MODULEMODE_EN  0x2U    /* MODULEMODE = ENABLE               */
#define CLKCTRL_IDLEST_MASK    (3U << 16) /* IDLEST field (must become 0x0) */

/* ------------------------------------------------------------------ */
/*  Register access helper                                              */
/* ------------------------------------------------------------------ */
#define REG32(base, off) \
    (*((volatile unsigned int *)((unsigned int)(base) + (unsigned int)(off))))

/* ------------------------------------------------------------------ */
/*  UART0 initialisation                                               */
/* ------------------------------------------------------------------ */
static void uart0_init(void)
{
    /* 1. Enable UART0 functional clock via CM_WKUP */
    REG32(CM_WKUP_BASE, CM_WKUP_UART0_CLKCTRL) = CLKCTRL_MODULEMODE_EN;
    /* Spin until the module is fully active (IDLEST == 0b00) */
    while (REG32(CM_WKUP_BASE, CM_WKUP_UART0_CLKCTRL) & CLKCTRL_IDLEST_MASK);

    /* 2. Reset / disable UART (MDR1 = 0x7 → disabled) */
    REG32(UART0_BASE, UART_MDR1) = 0x7U;

    /* 3. Enter config mode A: set LCR[7]=1 to access DLL/DLH
     *    0x83 = 1000_0011b  (DLAB=1, 8 data bits, no parity, 1 stop bit) */
    REG32(UART0_BASE, UART_LCR) = 0x83U;

    /* 4. Program baud-rate divisor for 115200 @ 48 MHz */
    REG32(UART0_BASE, UART_DLL) = 26U;  /* 0x1A */
    REG32(UART0_BASE, UART_DLH) = 0U;

    /* 5. Clear DLAB, configure 8N1
     *    0x03 = 0000_0011b  (8 data bits, no parity, 1 stop bit) */
    REG32(UART0_BASE, UART_LCR) = 0x03U;

    /* 6. Disable UART interrupts */
    REG32(UART0_BASE, UART_IER) = 0x0U;

    /* 7. Enable FIFOs, clear TX & RX FIFOs (FCR = 0x07) */
    REG32(UART0_BASE, UART_FCR) = 0x07U;

    /* 8. Enable UART in UART 16× mode (MDR1 = 0x0) */
    REG32(UART0_BASE, UART_MDR1) = 0x0U;
}

/* ------------------------------------------------------------------ */
/*  UART I/O                                                           */
/* ------------------------------------------------------------------ */
void uart_putc(char c)
{
    /* Wait until the TX holding register is empty */
    while (!(REG32(UART0_BASE, UART_LSR) & UART_LSR_THRE));
    REG32(UART0_BASE, UART_THR) = (unsigned int)c;
}

void uart_puts(const char *s)
{
    while (*s) {
        uart_putc(*s++);
    }
}

/* ------------------------------------------------------------------ */
/*  Entry point                                                        */
/* ------------------------------------------------------------------ */
void main(void)
{
    uart0_init();
    uart_puts("Hello World\n");
    while (1);  /* Spin forever — nothing to return to */
}
