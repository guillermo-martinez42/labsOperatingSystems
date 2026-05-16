/*
 * startup_bbb.s - Minimal startup for BeagleBone Black via U-Boot loady/go
 *
 * Same rationale as lab01bb/startup_bbb.s:
 *  - .arm directive ensures 32-bit ARM encoding (not Thumb)
 *  - No vector table, no VBAR write, no CPSR mode change
 *    (all three caused a prefetch abort at load_addr-4 in the old version)
 *  - Target DDR (0x80000000) not SRAM
 *
 * Load address : 0x80000000 (DDR)
 * Stack top    : 0x80010000
 */

.syntax unified
.arm

.section .text.startup
.global _start

_start:
    ldr     sp, =0x80010000

    ldr     r0, =__bss_start
    ldr     r1, =__bss_end
    mov     r2, #0
_bss_loop:
    cmp     r0, r1
    strlt   r2, [r0], #4
    blt     _bss_loop

    bl      main

_hang:
    b       _hang
