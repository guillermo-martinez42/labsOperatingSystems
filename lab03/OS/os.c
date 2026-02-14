#include "os.h"

// BeagleBone Black UART0 base address
#define UART0_BASE     0x10009000 //0x44E09000
#define UART_THR       (UART0_BASE + 0x00)  // Transmit Holding Register
#define UART_LSR       (UART0_BASE + 0x18)  // Line Status Register
#define UART_LSR_TXFF  0x20                  // Transmit Holding Register Empty
#define UART_LSR_RXFE  0x10                  // Receive FIFO Empty


#define TIMER0_BASE    0x10011000
#define TIMER_LOAD     (TIMER0_BASE + 0x00)
#define TIMER_VALUE    (TIMER0_BASE + 0x04)
#define TIMER_CONTROL  (TIMER0_BASE + 0x08)
#define TIMER_INTCLR   (TIMER0_BASE + 0x0C)

// Generic Interrupt Controller (GIC)
#define GIC_DIST_BASE  0x1E001000
#define GIC_CPU_BASE   0x1E000000

// GIC Distributor Registers
#define GIC_DIST_CTRL    (*(volatile unsigned int*)(GIC_DIST_BASE + 0x000))
#define GIC_DIST_ENABLE1 (*(volatile unsigned int*)(GIC_DIST_BASE + 0x104)) // IRQ 32-63

// GIC CPU Interface Registers
#define GIC_CPU_CTRL     (*(volatile unsigned int*)(GIC_CPU_BASE + 0x000))
#define GIC_CPU_PRIMASK  (*(volatile unsigned int*)(GIC_CPU_BASE + 0x004))
#define GIC_CPU_INTACK   (*(volatile unsigned int*)(GIC_CPU_BASE + 0x00C))
#define GIC_CPU_EOI      (*(volatile unsigned int*)(GIC_CPU_BASE + 0x010))

// Timer 0 is connected to IRQ 44 on RealView PB-A8
#define TIMER_IRQ_OFFSET (44 - 32) // Bit offset in ENABLE1 register


#define VIC_BASE       0x10040000
#define VIC_INTENABLE  (VIC_BASE + 0x10)

// BeagleBone Black DMTIMER2 base address
#define DMTIMER2_BASE    0x48040000
#define TCLR             (DMTIMER2_BASE + 0x38)  // Timer Control Register
#define TCRR             (DMTIMER2_BASE + 0x3C)  // Timer Counter Register
#define TISR             (DMTIMER2_BASE + 0x28)  // Timer Interrupt Status Register
#define TIER             (DMTIMER2_BASE + 0x2C)  // Timer Interrupt Enable Register
#define TLDR             (DMTIMER2_BASE + 0x40)  // Timer Load Register

// BeagleBone Black Interrupt Controller (INTCPS) base address
#define INTCPS_BASE      0x48200000
#define INTC_MIR_CLEAR2  (INTCPS_BASE + 0xC8)    // Interrupt Mask Clear Register 2
#define INTC_CONTROL     (INTCPS_BASE + 0x48)    // Interrupt Controller Control
#define INTC_ILR68       (INTCPS_BASE + 0x110)   // Interrupt Line Register 68

// Clock Manager base address
#define CM_PER_BASE      0x44E00000
#define CM_PER_TIMER2_CLKCTRL (CM_PER_BASE + 0x80)  // Timer2 Clock Control

// ============================================================================
// UART Functions
// ============================================================================

// Function to send a single character via UART
void uart_putc(char c) {
    // Wait until Transmit Holding Register is empty
    while ((GET32(UART_LSR) & UART_LSR_TXFF) != 0);
    PUT32(UART_THR, c);
}

// Function to receive a single character via UART
char uart_getc(void) {
    // Wait until data is available
    while ((GET32(UART_LSR) & UART_LSR_RXFE) != 0);
    return (char)(GET32(UART_THR) & 0xFF);
}

// Function to send a string via UART
void os_write(const char *s) {
    while (*s) {
        uart_putc(*s++);
    }
}

// Function to receive a line of input via UART
void os_read(char *buffer, int max_length) {
    int i = 0;
    char c;
    while (i < max_length - 1) { // Leave space for null terminator
        c = uart_getc();
        if (c == '\n' || c == '\r') {
            uart_putc('\n'); // Echo newline
            break;
        }
        uart_putc(c); // Echo character
        buffer[i++] = c;
    }
    buffer[i] = '\0'; // Null terminate the string
}

// Helper function to print an unsigned integer
void uart_putnum(unsigned int num) {
    char buf[10];
    int i = 0;
    if (num == 0) {
        uart_putc('0');
        uart_putc('\n');
        return;
    }
    do {
        buf[i++] = (num % 10) + '0';
        num /= 10;
    } while (num > 0 && i < 10);
    while (i > 0) {
        uart_putc(buf[--i]);
    }
    uart_putc('\n');
}

// ============================================================================
// Timer Functions
// ============================================================================

// TODO: Implement timer initialization
// This function should:
// 1. Enable the timer clock (CM_PER_TIMER2_CLKCTRL = 0x2)
// 2. Unmask IRQ 68 in the interrupt controller (INTC_MIR_CLEAR2)
// 3. Configure interrupt priority (INTC_ILR68 = 0x0)
// 4. Stop the timer (TCLR = 0)
// 5. Clear any pending interrupts (TISR = 0x7)
// 6. Set the load value for 2 seconds (TLDR = 0xFE91CA00)
// 7. Set the counter to the same value (TCRR = 0xFE91CA00)
// 8. Enable overflow interrupt (TIER = 0x2)
// 9. Start timer in auto-reload mode (TCLR = 0x3)
void timer_init(void) {
	// Set load value (for approximately 2 seconds depending on clock)
    PUT32(TIMER_LOAD, 0x100000);

    // Timer Control: Enable (bit 7), Periodic (bit 6), Interrupt Enable (bit 5), 32-bit (bit 1)
    PUT32(TIMER_CONTROL, 0xE2);
    // 2. Configure GIC (Interrupt Controller)
    // Enable the GIC Distributor
    GIC_DIST_CTRL = 1;

    // Unmask Timer Interrupt (IRQ 44)
    // IRQ 44 is in the second bank (ENABLE1), so we set bit (44-32) = 12
    GIC_DIST_ENABLE1 |= (1 << 4);

    // 3. Configure CPU Interface
    // Set Priority Mask to 0xFF (allow all priorities)
    GIC_CPU_PRIMASK = 0xFF;

    // Enable CPU Interface
    GIC_CPU_CTRL = 1;

  /* // 1. Enable the timer clock (CM_PER_TIMER2_CLKCTRL = 0x2)
    PUT32(CM_PER_TIMER2_CLKCTRL, 0x2);

	// 2. Unmask IRQ 68 in the interrupt controller (INTC_MIR_CLEAR2)
    PUT32(INTC_MIR_CLEAR2, (1 << 4));

    // 3. Configure interrupt priority (INTC_ILR68 = 0x0)
    PUT32(INTC_ILR68, 0x0);

    // 4. Stop the timer (TCLR = 0)
    PUT32(TCLR, 0);

    // 5. Clear any pending interrupts (TISR = 0x7)
    PUT32(TISR, 0x7);

    // 6. Set the load value for 2 seconds (TLDR = 0xFE91CA00)
    PUT32(TLDR, 0xFE91CA00);

    // 7. Set the counter to the same value (TCRR = 0xFE91CA00)
    PUT32(TCRR, 0xFE91CA00);

    // 8. Enable overflow interrupt (TIER = 0x2)
    PUT32(TIER, 0x2);


    PUT32(TCLR, 0x3);*/

}

// TODO: Implement timer interrupt handler
// This function should:
// 1. Clear the timer interrupt flag (TISR = 0x2)
// 2. Acknowledge the interrupt to the controller (INTC_CONTROL = 0x1)
// 3. Print "Tick\n" via UART    PUT32(TISR, 0x2);     PUT32(INTC_CONTROL, 0x1);
void timer_irq_handler(void) {
    // 1. Clear the Timer Interrupt
    PUT32(TIMER_INTCLR, 1);

    // 2. Acknowledge the Interrupt at the GIC
    // This tells the GIC we are handling it and gives us the ID
    unsigned int ack_id = GIC_CPU_INTACK;

    // 3. Print Tick
    os_write("Tick!!!!!!!!!!!\n");

    // 4. Signal End of Interrupt (EOI) to GIC
    // This tells the GIC we are done, so it can send the next one
    GIC_CPU_EOI = ack_id;
}

void enable_irq(void) {
    asm volatile (
        "mrs r0, cpsr      \n\t"
        "bic r0, r0, #0x80 \n\t"
        "msr cpsr_c, r0    \n\t"
        :
        :
        : "r0", "memory"
    );
}

// ============================================================================
// Main Program
// ============================================================================

// Simple random number generator (Linear Congruential Generator)
static unsigned int seed = 12345;

unsigned int rand(void) {
    seed = (seed * 1103515245 + 12345) & 0x7fffffff;
    return seed;
}

int main(void) {
    // TODO: Print initialization message
	os_write("Starting system\n");
    // TODO: Initialize the timer using timer_init()
	timer_init();
    // TODO: Enable interrupts using enable_irq()
	enable_irq();
    // TODO: Print a message indicating interrupts are enabled
	os_write("Interrupts are enabled\n");


    // Main loop: continuously print random numbers
    while (1) {
        unsigned int random_num = rand() % 1000;
        uart_putnum(random_num);
        
        // Small delay to prevent overwhelming UART
        for (volatile int i = 0; i < 100000000; i++);
    }
    
    return 0;
}
