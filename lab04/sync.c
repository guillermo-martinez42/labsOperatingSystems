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
        pid_t child_pid = waitpid(pid, &status, 0);
        if (WIFEXITED(status)) {
            printf("Parent Process: Child %d exited with status: %d\n", child_pid, WEXITSTATUS(status));
        }
    }
}
