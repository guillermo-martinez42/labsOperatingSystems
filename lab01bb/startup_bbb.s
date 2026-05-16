/*
 * startup_bbb.s - Minimal startup for BeagleBone Black via U-Boot loady/go
 *
 * U-Boot already runs in SVC mode with caches, MMU, and UART0 set up.
 * All we need to do here is:
 *   1. Establish our own stack pointer (so we don't corrupt U-Boot's stack)
 *   2. Zero-fill the BSS section
 *   3. Branch to main()
 *
 * NO vector table, NO VBAR write, NO CPSR mode change.
 * Those caused a prefetch abort at (load_addr - 4) in the previous version
 * because the VBAR/CPSR writes conflicted with U-Boot's live exception state.
 *
 * Load address : 0x80000000 (DDR — same region U-Boot uses)
 * Stack top    : 0x80010000 (64 KB above the load address)
 */

.syntax unified       /* allow both ARM and Thumb syntax in one file */
.arm                  /* emit 32-bit ARM instructions — critical;     */
                      /* without this some toolchains default to Thumb */

.section .text.startup
.global _start

_start:
    /*------------------------------------------------------------------
     * 1. Set up the stack (grows downward from 0x80010000).
     *------------------------------------------------------------------*/
    ldr     sp, =0x80010000

    /*------------------------------------------------------------------
     * 2. Zero-fill BSS.
     *    __bss_start / __bss_end are provided by the linker script.
     *------------------------------------------------------------------*/
    ldr     r0, =__bss_start
    ldr     r1, =__bss_end
    mov     r2, #0
_bss_loop:
    cmp     r0, r1
    strlt   r2, [r0], #4
    blt     _bss_loop

    /*------------------------------------------------------------------
     * 3. Call the C entry point.
     *------------------------------------------------------------------*/
    bl      main

    /* If main() ever returns, spin here. */
_hang:
    b       _hang
