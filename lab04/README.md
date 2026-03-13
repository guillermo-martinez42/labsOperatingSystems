# Lab04 - Process Creation and Management

#### By @GabGP and @guillermo-martinez42

This project aims to demonstrate different process creations and management with 5 different small programs.

Namely:

- Creating a New Process (fork.c)
- Synchronizing Parent and Child Process (sync.c)
- Inter-Process Communication Using Pipes (ipc.c)
- Creating Multiple Child Processes (mcp.c)
- Shared Memory (shmem.c)

## 0. How to Run?

This program can be ran by running each executable.

Alternatively, each C file can be compiled using gcc like this:

`gcc file.c -o file`

And then running the executable.

Note: These programs are meant to be ran on Linux.

## 1. Process Creation Steps

Once the program starts running, the OS creates a process for this task. This initial process gets named the Parent Process.

### Creating a New Process (fork.c)

On Linux using C we can create processes that stem from the Parent using fork(). This will create a copy of the Parent Process and run the program on its own separate from the Parent. This new process is named the Child Process.

### Creating Multiple Child Processes (mcp.c)

Following this same idea, Mutiple Child Processes can be created by the same Parent Process. In this case, this gets showcased by using a for loop creating 3 Child Processes. 

Note how in both programs the behaviour of the program is different depending on whether the process is a Parent or Child.

Further read: https://pubs.opengroup.org/onlinepubs/009696799/functions/fork.html

## 2. How Does Synchronization Work?

### Synchronizing Parent and Child Process (sync.c)

On this program it is showcased the synchronization between the child and parent process.

This gets achieved by the parent waiting on the child process to be terminated, so that the parent process can terminate as well.

The Parent Process shares a pointer to an integer variable, so that the child process can use this pointer to share its status.

Additionally, the parent proecss is waiting on any child process. This is given, when the options value is 0 on waitpid().

Further read: https://linux.die.net/man/2/waitpid

## 3. How Does The IPC Mechanism Work?

The IPC or Inter-Process Communication is a mechanism, where 2 or more processes can communicate with each other.

On this project this gets achieved by using pipes and shared memory.

### Inter-Process Communication Using Pipes (ipc.c)

This type of communication is just one-way and this can be seen, when the parent writes. Then the child process can only read this information.

When a pipe gets created, it creates 2 ends. One for the parent and one for the child.

Once the pipe gets created, the parent can continue writing through the pipe to the child and child can read this information.

The pipe works as a type of queue (FIFO), so any information inputed first will get read first.

Further read: 
- https://www.geeksforgeeks.org/c/pipe-system-call/
- https://pubs.opengroup.org/onlinepubs/009604599/functions/pipe.html

### Shared Memory (shmem.c)
In the case of shared memory, we have a two-way communication, however, for this program the communication works just one-way.

First, a shared memory ID gets created using shmget(), this is important, so that both the parent and child process can communicate with each other using the same SHMID.

To add to that, it is important to set the proper permits, so that it can be written and read. To do this, we follow the rwx method for user, group and other, and to create the SHMID, it is necessary to give the IPC_CREAT flag as well.

Once the SHMID is created, a memory space can be reserved giving the SHMID, address and flag using shmat().

The flags used on this program are SHM_RDONLY for read-only memory and 0 for read and write. Also, in this case, the address is NULL, this way the system will choose a proper memory segment.

Once this gets created successfully, shmat() will return an address for the shared memory space. Then we can either write or read to this address regardless of the process. Given that it has the proper permits to do so, of course.

Further read: 
- https://man7.org/linux/man-pages/man2/shmget.2.html
- https://man7.org/linux/man-pages/man2/shmat.2.html
