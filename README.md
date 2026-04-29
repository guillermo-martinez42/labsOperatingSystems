# Operating Systems Labs

Welcome to the **Operating Systems Labs** repository! This collection of assignments spans from low-level, bare-metal ARM architecture programming to high-level process management and concurrent thread synchronization.

---

## 🔬 Lab Overview

| Lab | Title | Core Concepts | Target Environment |
| :--- | :--- | :--- | :--- |
| **Lab 001** | Hello World (Qemu) | Bare-metal programming, UART, Cross-compilation | ARM / QEMU |
| **Lab 002** | Calculator | Abstraction layers, Custom I/O (`printf`/`scanf`), Hardware Interfacing | ARM / QEMU |
| **Lab 003** | Interrupts and Exceptions | Timer Peripherals, Interrupt Vector Table (IVT), ISRs | ARM / QEMU |
| **Lab 004** | Process Management | `fork()`, `exec()`, IPC (Pipes/Shared Memory), Synchronization | Linux |
| **Lab 005** | Threads | Pthreads, Multi-threading, Log Parsing | Linux |
| **Lab 006** | Semaphores | Counting semaphores, Mutex locks, Shared resources | Linux |
| **Lab 007** | Condition Variables | Thread coordination, Complex synchronization logic | Linux |
| **Lab 008** | CPU Scheduling | FIFO, SJF, Round Robin, SRTF, Mutex synchronization | Linux |

---

## 🛠️ Detailed Lab Descriptions

### Lab 001: Hello World - Qemu

An introduction to **bare-metal programming**. This lab focuses on cross-compiling code for ARM Cortex processors and interacting with hardware interfaces to output a simple string via the serial console using QEMU.

### Lab 002: Calculator

Builds upon the bare-metal foundation by dividing application logic into **User**, **Language Library**, and **OS/Hardware** layers. You will implement basic I/O functions analogous to standard C library features without actually using the standard library.

### Lab 003: Interrupt and Exception Handling

Transitions from busy-wait loops to **interrupt-driven programming**. This lab explores configuring ARM timer peripherals and servicing hardware interrupts directly in assembly.

### Lab 004: Process Creation and Management

Explores the fundamental system calls for **process creation and management** (`fork()`, `CreateProcess()`). You will implement process synchronization and Inter-Process Communication (IPC) using pipes and shared memory.

### Lab 005: Threads (Multi-Threaded Web Log Analyzer)

Focuses on using **multi-threading** to optimize computation time. The challenge involves parsing a massive web access log in parallel to extract metrics like unique IPs, top URLs, and error counts safely.

### Lab 006: Semaphores (Smart Parking Lot System)

Introduces **counting semaphores** to manage a limited shared resource. You will coordinate multiple "car" threads arriving and departing from a capacity-constrained parking lot while strictly avoiding race conditions.

### Lab 007: Condition Variables (Zombie Bridge Crossing Challenge)

A complex concurrency challenge requiring **condition variables** and **mutex locks**. You will manage directional traffic across a narrow bridge, ensuring maximum capacity rules and direction synchronization are enforced to avoid the zombie horde.

### Lab 008: CPU Scheduling Algorithms

Simulates various CPU scheduling strategies using multi-threading. This lab covers non-preemptive algorithms (FIFO, SJF) and preemptive ones (Round Robin, SRTF), focusing on performance metrics like Waiting Time and Turnaround Time using thread-safe queues.

---

## ⚙️ Prerequisites

To run these labs, ensure you have the following toolchains installed:

* **ARM GNU Toolchain**: `arm-none-eabi-gcc` for compiling bare-metal ARM code.
* **QEMU**: Specifically `qemu-system-arm` for emulating the versatilepb machine.
* **Native C Compiler**: `gcc` or `clang` for Linux/Windows user-space labs.
* **Make / Bash**: For running build scripts and Makefiles.

> **Pro-Tip:** For the bare-metal labs (001-003), pay close attention to the linker scripts (`.ld`) as they are critical for correctly mapping your program into emulated memory.