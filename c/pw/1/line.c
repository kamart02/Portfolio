#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#include "err.h"

#define N_PROC 5
#define BUFFER_SIZE 64

int main(int argc, char* argv[]) {

    int n_children = atoi(argv[1]);

    pid_t pid;
    if (n_children != 0) {
        ASSERT_SYS_OK(pid = fork());
    }
    else {
        pid = -1;
    }

    if (pid != 0) {
        printf("My pid is %d, my parent\'s pid is %d\n", getpid(), getppid());
        if (pid != -1) {
            ASSERT_SYS_OK(wait(NULL));
        }
    }
    else {
        if (n_children != 0) {
            char buffer[BUFFER_SIZE];
            int ret = snprintf(buffer, sizeof buffer, "%d", n_children - 1);
            if (ret < 0 || ret >= (int)sizeof(buffer))
                fatal("snprintf failed");
            ASSERT_SYS_OK(execlp(argv[0], argv[0], buffer, NULL));
        }
    }

    return 0;
}