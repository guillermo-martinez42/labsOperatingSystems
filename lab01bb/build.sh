#!/bin/bash
# build.sh - Build lab01bb for BeagleBone Black (AM335x Cortex-A8)
#
# Requirements (WSL):
#   sudo apt install gcc-arm-none-eabi binutils-arm-none-eabi
#
# Output:
#   hello_bbb.elf  - ELF with debug info (readable by GDB)
#   hello_bbb.bin  - flat binary for SD-card deployment
#
# Deploy to BBB (via SD card MLO):
#   See README below or use the TI AM335x MLO tool to wrap hello_bbb.bin.

set -e

CROSS=arm-none-eabi
CPU_FLAGS="-mcpu=cortex-a8"
COMMON_FLAGS="${CPU_FLAGS} -ffreestanding -nostdlib -nostartfiles -Wall -Wextra"

echo "=== lab01bb: BeagleBone Black bare-metal Hello World ==="

echo "[1/3] Assembling startup_bbb.s ..."
${CROSS}-as ${CPU_FLAGS} -o startup_bbb.o startup_bbb.s

echo "[2/3] Compiling hello_bbb.c ..."
${CROSS}-gcc ${COMMON_FLAGS} -c -o hello_bbb.o hello_bbb.c

echo "[3/3] Linking ..."
${CROSS}-gcc ${COMMON_FLAGS} -T memmap_bbb.ld \
    startup_bbb.o hello_bbb.o \
    -lgcc \
    -o hello_bbb.elf

echo "Converting ELF to flat binary ..."
${CROSS}-objcopy -O binary hello_bbb.elf hello_bbb.bin

echo ""
echo "Build complete:"
${CROSS}-size hello_bbb.elf

echo ""
echo "--- Memory map (load at 0x402F0400, stack top 0x4030FF00) ---"
${CROSS}-nm --numeric-sort hello_bbb.elf | grep -E "(_start|main|__bss|__stack)" || true

echo ""
echo "To load onto BBB (at U-Boot prompt, minicom 115200):"
echo "  loady 0x80000000          # then send hello_bbb.bin via YMODEM"
echo "  go    0x80000000"
