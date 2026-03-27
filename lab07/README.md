# Lab07 - Condition Variables (Zombie Bridge Crossing Challenge)

#### By @GabGP and @guillermo-martinez42

This project aims to demonstrate the usage of condition variables and mutex locks with multi-threading programming by simulating a challenging bridge crossing scenario.

Functions:

- Simulate students crossing a narrow bridge with limited capacity.
- Implement a priority mechanism for students who have been waiting for an extended period.
- Implement a starvation prevention mechanism to ensure no direction waits indefinitely.
- Print logs of the simulation

![Alt text](https://github.com/guillermo-martinez42/labsOperatingSystems/blob/main/lab06/Screenshots/example_console.png "Expected Output")

## 0. How to Run?

This program can be built and ran by running the shell script `build_and_run.sh`.

Note: This program is meant to be ran on Linux.

### Libraries Required

- `pthread` for multi-threading and synchronization.

## 1. Zombie Bridge Crossing Challenge System (zombie_bridge.c)

### Simulate Student Threads
Each student is represented by a thread, simulating their journey to cross the bridge.

The behavior of each student thread (`student_thread`) includes:

Arrival:

- The student arrives at one side of the bridge, logs their arrival, and attempts to gain access.

Waiting:

- If the bridge is full, moving in the opposite direction, or if synchronization rules dictate, the student waits using a condition variable.

Crossing:

- Once conditions are met, the student enters the bridge, logs their entry, and simulates crossing time.

Leaving:

- After crossing, the student exits the bridge and logs their departure.

### Synchronization (Mutexes and Condition Variables)

The `Bridge` struct encapsulates the shared state of the bridge, including the number of students currently on it, the current crossing direction, and counts of waiting students.

A `pthread_mutex_t` (`bridge.mutex`) is used to protect all shared data within the `Bridge` struct, ensuring atomic updates and preventing race conditions.

A `pthread_cond_t` (`bridge.condition`) is used to coordinate student threads. Students wait on this condition variable when they cannot cross, and are signaled when the bridge state changes (e.g., a student exits, making space or allowing a direction switch). 

The `pthread_cond_broadcast` is used to wake all waiting threads, allowing them to re-evaluate their crossing conditions.

### Priority Mechanism

Students who have been waiting for a duration exceeding `PRIORITY_WAIT_THRESHOLD` (e.g., 5 seconds) are marked as "priority waiters." The students who doesn't have priority and other priority waiters are on the same crossing direction, then these unprioritized students yield.

### Starvation Prevention

To prevent indefinite waiting for one side, the crossing direction changes once the bridge is empty.

Further read:
- Pthreads Condition Variables
https://www.ibm.com/docs/en/zos/2.5.0?topic=functions-pthread-cond-wait-wait-condition-variable
- Pthreads Overview
https://www.geeksforgeeks.org/c/thread-functions-in-c-c/

## 2. Main Program

The `main` function initializes and creates `NUM_STUDENTS` threads, each simulating a student. It then waits for all student threads to complete their crossing simulation before exiting.
