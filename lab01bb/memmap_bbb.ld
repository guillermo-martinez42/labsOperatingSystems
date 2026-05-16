/*
 * memmap_bbb.ld - Linker script for BeagleBone Black via U-Boot loady/go
 *
 * Target: DDR3 SDRAM, which starts at 0x80000000 on the AM335x.
 * U-Boot lives at the TOP of DDR (~0x9FF00000 after relocation).
 * Loading at 0x80000000 is well clear of U-Boot and is the standard
 * address used by U-Boot's own loadaddr / loady workflow.
 *
 * Why DDR instead of internal SRAM (0x402F0400)?
 *   - U-Boot runs from DDR; the MMU/cache configuration there is known.
 *   - Internal SRAM 0x402F0000-0x402F03FF is reserved by the ROM
 *     bootloader; accessing it as executable caused a prefetch abort.
 *   - Standard BBB bare-metal tutorials universally target DDR for
 *     loady/go use.
 */

ENTRY(_start)

MEMORY
{
    /*         Origin       Length  */
    DDR (rwx) : ORIGIN = 0x80000000, LENGTH = 512M
}

SECTIONS
{
    /* Executable code — .text.startup must be first so _start is
       placed at the very first byte of the binary (= load address). */
    .text :
    {
        *(.text.startup)
        *(.text*)
        *(.rodata*)
    } > DDR

    /* Initialised data */
    .data : AT (ADDR(.text) + SIZEOF(.text))
    {
        __data_start = .;
        *(.data*)
        __data_end   = .;
    } > DDR

    /* Zero-initialised data (cleared by startup code) */
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
