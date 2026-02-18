
#define _POSIX_SOURCE
#include <sys/types.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>

#define MSGSIZE 32
char* msg = "Hello from Parent";




int main() {

    pid_t pid;
    int status;
    char inbuf[MSGSIZE];
    int p[2], i;

    if (pipe(p))
    {
        perror("pipe failed");
        exit(1);
    }

    if ((pid = fork()) < 0)
        perror("fork() error");
    else if (pid == 0) {
        sleep(5);
        puts("This is the child.");
        printf("Child Process: PID= %d , Parent PID= %d\n", (int) getpid(), (int) getppid());
        read(p[0], inbuf, MSGSIZE);
        printf("Child Process: Reading \"%s\"\n", inbuf);
        exit(42);
    }
    else {
        puts("This is the parent.");
        printf("Parent Process: PID= %d \n", (int) getpid());
        write(p[1], msg, MSGSIZE);
        printf("Parent Process: Writing \"%s\"\n", msg);
        pid_t child_pid = waitpid(pid, &status, 0);
        if (WIFEXITED(status)) {
            printf("Parent Process: Child %d exited with status: %d\n", child_pid, WEXITSTATUS(status));
        }
    }
}