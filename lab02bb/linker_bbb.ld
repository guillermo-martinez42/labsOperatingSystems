/*
 * linker_bbb.ld - Linker script for BeagleBone Black via U-Boot loady/go
 *
 * Target: DDR3 at 0x80000000 (512 MB available on BBB).
 *
 * Why DDR instead of internal SRAM (0x402F0400)?
 *   The old SRAM target caused a prefetch abort at (0x402F0400 - 4)
 *   because 0x402F0000-0x402F03FF is reserved/protected by the ROM
 *   bootloader and the VBAR/CPSR writes in the old startup conflicted
 *   with U-Boot's live exception state.
 *   DDR is the standard target for U-Boot loady/go bare-metal code.
 */

ENTRY(_start)

MEMORY
{
    DDR (rwx) : ORIGIN = 0x80000000, LENGTH = 512M
}

SECTIONS
{
    .text :
    {
        *(.text.startup)
        *(.text*)
        *(.rodata*)
    } > DDR

    .data : AT (ADDR(.text) + SIZEOF(.text))
    {
        __data_start = .;
        *(.data*)
        __data_end   = .;
    } > DDR

    .bss :
    {
        . = ALIGN(4);
        __bss_start = .;
        *(.bss*)
        *(COMMON)
        . = ALIGN(4);
        __bss_end = .;
    } > DDR
}
