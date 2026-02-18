
#define _POSIX_SOURCE
#include <sys/types.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <sys/shm.h>
#include <string.h>

#define MSGSIZE 32
char *addr, *msg = "Shared Memory Example";

int main()
{
    pid_t pid;
    int status, shmid;

    // Creates the Shared Memory ID with the proper permits
    // IPC_CREAT: Create a new segment
    // PERMITS: User: rw- (6) Group: --- (0) Other: --- (0) = 600 (Octal)
    shmid = shmget(IPC_PRIVATE, (size_t)MSGSIZE, IPC_CREAT | 0600);
    if (shmid == -1)
    {
        perror("shmget() error");
        exit(EXIT_FAILURE);
    }

    if ((pid = fork()) < 0)
        perror("fork() error");
    else if (pid == 0)
    {
        sleep(5);
        puts("This is the child.");
        printf("Child Process: PID= %d , Parent PID= %d\n", (int)getpid(), (int)getppid());
        printf("Child Process: SHMID %d\n", shmid);

        // Gets the memory address at the created SHMID as read-only
        addr = shmat(shmid, NULL, SHM_RDONLY);
        if (addr == (void *)-1)
        {
            perror("Child Process: shmat() error");
            exit(EXIT_FAILURE);
        }

        printf("Child Process: Reading \"%s\"\n", addr);
        exit(42);
    }
    else
    {
        puts("This is the parent.");
        printf("Parent Process: PID= %d \n", (int)getpid());
        printf("Parent Process: SHMID %d\n", shmid);

        // Gets the memory address at the created SHMID as write and read
        addr = shmat(shmid, NULL, 0);
        if (addr == (void *)-1)
        {
            perror("Parent Process: shmat() error");
            exit(EXIT_FAILURE);
        }

        // Writes to the shared memory
        memcpy(addr, msg, MSGSIZE);
        printf("Parent Process: Writing \"%s\"\n", addr);

        pid_t child_pid = waitpid(pid, &status, 0);
        if (WIFEXITED(status))
        {
            printf("Parent Process: Child %d exited with status: %d\n", child_pid, WEXITSTATUS(status));
        }
    }
}