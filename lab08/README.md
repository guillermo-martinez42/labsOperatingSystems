# Lab08 - CPU Scheduling Algorithms

#### By @GabGP and @guillermo-martinez42

This project aims to simulate and analyze the behavior of various CPU scheduling algorithms using multi-threading to represent process arrivals and execution.

Functions:

- Simulate process arrivals using concurrent threads.
- Implement non-preemptive algorithms: First-In-First-Out (FIFO) and Shortest Job First (SJF).
- Implement preemptive algorithms: Round Robin (RR) and Shortest Remaining Time First (SRTF).
- Calculate performance metrics including Waiting Time and Turnaround Time.
- Detailed logging of arrival, execution, preemption, and completion events.

![Alt text](https://github.com/guillermo-martinez42/labsOperatingSystems/blob/main/lab08/Screenshots/fifo.png "FIFO")

![Alt text](https://github.com/guillermo-martinez42/labsOperatingSystems/blob/main/lab08/Screenshots/round_robin.png "Round Robin")

![Alt text](https://github.com/guillermo-martinez42/labsOperatingSystems/blob/main/lab08/Screenshots/sjf.png "SJF (Shortest Job First)")

![Alt text](https://github.com/guillermo-martinez42/labsOperatingSystems/blob/main/lab08/Screenshots/srtf.png "SRTF (Shortest Remaining Time First)")

## 0. How to Run?

This program can be compiled using `gcc`:

`gcc main.c scheduler/scheduler.c scheduler/queue.c logs/logger.c process/process.c -o scheduling -lpthread`

Alternatively, run the provided build script.

Note: This program is meant to be ran on Linux.

### Libraries Required

- `pthread` for multi-threading and synchronization.

## 1. CPU Scheduling Simulation

The system simulates a CPU environment where processes (represented by `Thread` structs) arrive at different times and require varying burst durations. 

### Arrival Simulation
Each process is managed by an independent `pthread`. These threads sleep for a duration equal to their `arrival_time` before placing themselves into the ready queue. This accurately simulates asynchronous arrivals in a real-time environment.

### Thread-Safe Ready Queue
The core of the scheduler is a shared `Queue`. Because multiple arrival threads and the main scheduler thread access this queue simultaneously, it is protected by a **Mutex Lock**. Every `enqueue`, `dequeue`, and `peek` operation is wrapped in a mutex lock/unlock pair to prevent race conditions and memory corruption.

## 2. Algorithms Implemented

### FIFO (First-In, First-Out)
A non-preemptive algorithm where processes are executed in the order they arrive.

### SJF (Shortest Job First)
A non-preemptive algorithm that selects the waiting process with the smallest initial burst time. The queue is managed using an `insert_SJF` function that maintains a sorted order.

### Round Robin (RR)
A preemptive algorithm that assigns a fixed time unit per process (Quantum). If a process does not complete within the quantum, it is preempted, re-enqueued, and the next process in the queue is selected.

### SRTF (Shortest Remaining Time First)
A preemptive version of SJF. The scheduler executes the process with the shortest remaining time. Preemption is implemented using a "tick" mechanism (10ms), after which the scheduler peeks at the ready queue to see if a newly arrived process has a shorter remaining time than the current one.

## 3. Synchronization and Statistics

### Mutex Usage
Mutexes are used extensively in `queue.c` to protect the linked-list pointers (`front` and `back`). Additionally, mutexes protect the logging system and the global statistics counters to ensure that metrics like Average Waiting Time are calculated based on consistent data.

### Metrics Calculation
- **Waiting Time**: The total time a process spends in the ready queue. For preemptive algorithms, this is calculated by subtracting the time spent on the CPU from the total time elapsed since arrival.
- **Turnaround Time**: The total time from arrival to completion.

Further read:
- Pthreads Overview
https://www.geeksforgeeks.org/c/thread-functions-in-c-c/

## 4. Main Program

The `main.c` file serves as the dispatcher. It initializes a random dataset of processes, resets their state between algorithm runs, and invokes the high-level runner functions:

1. `run_fifo()`
2. `run_rr()`
3. `run_sjf()`
4. `run_srtf()`

The scheduler logic is modularized, with runner functions centralized in `scheduler.c` and specific algorithm implementations contained in dedicated source files.
