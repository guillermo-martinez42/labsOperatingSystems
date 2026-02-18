//
// Created by gdavd on 15-Feb-26.
//

#define _POSIX_SOURCE
#include <sys/types.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>

int main() {
    //pid_t data type used to represent process IDs, process group IDs, and session IDs
    pid_t pid;
    int status;

    if ((pid = fork()) < 0)
        perror("fork() error");
    else if (pid == 0) {
        sleep(5);
        puts("This is the child.");
        printf("Child Process: PID= %d , Parent PID= %d\n",
               (int) getpid(), (int) getppid());
        exit(42);
    }
    else {
        puts("This is the parent.");
        printf("Parent Process: PID= %d \n",
               (int) getpid());

        if (wait(&status) == -1)
            perror("Parent Process: wait() error");
        else if (WIFEXITED(status))  //WIFEXITED is a Portable Operating System Interface macro used to  determine if a child process terminated normally
            printf("Parent Process: The child exited with status of %d\n",
                   WEXITSTATUS(status));
        else
            puts("Parent Process: The child did not exit successfully");
    }
}
