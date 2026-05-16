#!/bin/bash
# build.sh - Build lab02bb (calculator) for BeagleBone Black (AM335x Cortex-A8)
#
# Requirements (WSL):
#   sudo apt install gcc-arm-none-eabi binutils-arm-none-eabi
#
# Output files:
#   calculadora_bbb.elf  - ELF with symbols (for GDB / objdump)
#   calculadora_bbb.bin  - flat binary for SD-card deployment

set -e

CROSS=arm-none-eabi
CPU_FLAGS="-mcpu=cortex-a8"
COMMON_FLAGS="${CPU_FLAGS} -ffreestanding -nostdlib -nostartfiles -Wall -Wextra"

echo "=== lab02bb: BeagleBone Black bare-metal Calculator ==="

echo "[1/5] Assembling startup_bbb.s ..."
${CROSS}-as ${CPU_FLAGS} -o startup_bbb.o startup_bbb.s

echo "[2/5] Compiling os_bbb.c ..."
${CROSS}-gcc ${COMMON_FLAGS} -c -o os_bbb.o os_bbb.c

echo "[3/5] Compiling stdio_bbb.c ..."
${CROSS}-gcc ${COMMON_FLAGS} -c -o stdio_bbb.o stdio_bbb.c

echo "[4/5] Compiling main_bbb.c ..."
${CROSS}-gcc ${COMMON_FLAGS} -c -o main_bbb.o main_bbb.c

echo "[5/5] Linking ..."
${CROSS}-gcc ${COMMON_FLAGS} -T linker_bbb.ld \
    startup_bbb.o os_bbb.o stdio_bbb.o main_bbb.o \
    -lgcc \
    -o calculadora_bbb.elf

echo "Converting ELF to flat binary ..."
${CROSS}-objcopy -O binary calculadora_bbb.elf calculadora_bbb.bin

echo ""
echo "Build complete:"
${CROSS}-size calculadora_bbb.elf

echo ""
echo "--- Memory map (load at 0x80000000, stack top 0x80010000) ---"
${CROSS}-nm --numeric-sort calculadora_bbb.elf | \
    grep -E "(_start|main|uart|__bss|PRINT|READ)" || true

echo ""
echo "To run interactively via U-Boot + minicom:"
echo "  1. Boot BBB to U-Boot prompt"
echo "  2. U-Boot: loady 0x80000000   (then send calculadora_bbb.bin via YMODEM)"
echo "  3. U-Boot: go    0x80000000"
