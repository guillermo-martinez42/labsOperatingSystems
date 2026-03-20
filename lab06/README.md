# Lab05 - Semaphores

#### By @GabGP and @guillermo-martinez42

This project aims to demonstrate the usage of semaphores with multi-threading programming by simulating a smart parking lot system.

Functions:

- Simulate a parking lot with N amount of cars,that attempt to park in M amount of parking spots.
- Print logs of the simulation
- A GUI that represents the parking behaviour
    - Note: A sleep was added between events to achieve a GUI representation, otherwise, events that ocurr in less than a second are not noticeable at all.
    Unfortunately, this changes how the behavior would look like without the sleeps.

![Alt text](https://github.com/guillermo-martinez42/labsOperatingSystems/blob/main/lab06/Screenshots/example_console.png "Expected Output")

![Alt text](https://github.com/guillermo-martinez42/labsOperatingSystems/blob/main/lab06/Screenshots/example_gui.png "Expected Output")

## 0. How to Run?

This program can be built and ran by running the shell script `build_and_run.sh` or the executable `parking_lot`.

Note: This program is meant to be ran on Linux.

### Libraries Required

- SDL2/SDL.h -- `sudo apt install libsdl2-dev`
- SDL2/SDL_ttf.h -- `sudo apt install libsdl2-ttf-dev`

## 1. Smart Parking Lot System (parking_lot.c)

### Simulate Car Threads

Each car would represent a thread, which has a behavior and on each step it logs its status. The behavior is:

Arrival:

- This is when the function is starting.

Waiting:

- This happens when the semaphore makes the thread wait.

Parking:

- This happens when the semaphore allows the thread to continue.

Leaving:

- This happens when the thread has done what it was programmed to do and is unlocked by the semaphore.

To better understand this behavior read the following section.

Further read: https://www.geeksforgeeks.org/c/thread-functions-in-c-c/

### Semaphore for Parking Spots

A certain amount of semaphores are created, that in this case represent a parking spot.

A car (thread) is said to be parked, when it is not waiting.

The semphare controls the ability of threads to do a work or not. This can be achieved with sem_wait() and sem_post().

In reality the semaphore is a type of counter, which doesn't allow the counter to go below zero.

When the semaphore is unlocked, it adds 1 to this counter, but when the semaphore sees wait, the semaphore can either allow the thread to continue, if by decrementing the counter doesn't go below zero. Otherwise, it makes the thread to wait until it is allowed to continue.

Further read:
- https://man7.org/linux/man-pages/man0/semaphore.h.0p.html
- https://faculty.cs.niu.edu/~hutchins/csci480/semaphor.htm

### Thread-Safe Logging

Each time a log is recorded, this is done employing a mutex lock (pthread_mutex_lock/unlock). This way the logs can be protected as they are shared with the threads.

### Statistics Tracking

Each time the statistics are updated, this is done employing a mutex lock (pthread_mutex_lock/unlock). This way the statistics can be protected as they are shared with the threads.

### GUI

This small GUI was created with SDL2 for the window and graphics, and SDL2_ttf for drawing text.

Is is important to mention that the drawings were made as well in a thread-safe eviroment.

The font used is Roboto by Google.

Further read:

- https://learncgames.com/tutorials/getting-started-with-sdl-and-c/
- https://gist.github.com/stavrossk/5004111
