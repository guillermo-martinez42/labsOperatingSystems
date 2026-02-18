//
//

#define _POSIX_SOURCE
#include <sys/types.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>

int main() {

    pid_t pid;
    int status,i;

    printf("Parent Process: PID= %d\n", (int)getpid());
    for (i = 0; i < 3; i++) {
    if ((pid = fork()) < 0)
        perror("fork() error");
    else if (pid == 0) {
            //sleep(5);
            printf("This is the child #%d \n",i);
            printf("Child Process: PID= %d , Parent PID= %d\n",(int) getpid(), (int) getppid());
            sleep(1);
            exit(i+1);
        }
    }

    //puts("This is the parent.");
    pid_t terminated_pid;
    while ((terminated_pid = waitpid(-1, &status, 0)) > 0) {
        if (WIFEXITED(status)) {
            printf("Parent Process: Child %d finished with exit code %d\n",
                    terminated_pid, WEXITSTATUS(status));
        }
    }

}
