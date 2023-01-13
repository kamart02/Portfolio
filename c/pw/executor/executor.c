#include "err.h"
#include "utils.h"

#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <semaphore.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/mman.h>

#define MAX_LINE_LENGTH 512
#define MAX_TASK_N 4096
#define MAX_TASK_OUT_LENGTH 1023

#define MMAP(x) mmap(NULL, x, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);

typedef struct Task {
    int stdReadFd;
    int errReadFd;
    int execPid;
    int handlerPid;
    char * lastStdLine;
    char * lastErrLine;
    sem_t mutex;
} Task;

int newTaskNumber = -1;
Task * tasks;

sem_t * globalMutex;

void bufferNextLine(int taskNumber, bool err) {
    int fd;
    char * lastLine;
    sem_t * mutex = &tasks[taskNumber].mutex;
    if (err) {
        fd = tasks[taskNumber].errReadFd;
        lastLine = tasks[taskNumber].lastErrLine;
    } else {
        fd = tasks[taskNumber].stdReadFd;
        lastLine = tasks[taskNumber].lastStdLine;
    }

    char * buffer = malloc(sizeof(char) * MAX_TASK_OUT_LENGTH);
    if (buffer == NULL) {
        fatal("Unable to create buffer");
    }
    char letter;
    int counter = 0;
    while (true) {
        int ret = read(fd, &letter, sizeof(char));
        if (ret == 0) {
            if (counter != 0) {
                buffer[counter] = '\0';
                ASSERT_SYS_OK(sem_wait(mutex));
                strcpy(lastLine, buffer);
                ASSERT_SYS_OK(sem_post(mutex));
            }
            break;
        }
        if (letter == '\n') {
            buffer[counter] = '\0';
            ASSERT_SYS_OK(sem_wait(mutex));
            strcpy(lastLine, buffer);
            ASSERT_SYS_OK(sem_post(mutex));
            counter = 0;
            continue;
        }
        buffer[counter++] = letter;
    }

    free(buffer);
}

void runHandler(int taskNumber, char ** splitInput, int splitInputLength) {
    int stdFd[2];
    int errFd[2];

    ASSERT_SYS_OK(pipe(stdFd));
    ASSERT_SYS_OK(pipe(errFd));

    int execPid = fork();
    ASSERT_SYS_OK(execPid);

    if (execPid == 0) {
        ASSERT_SYS_OK(dup2(stdFd[1], STDOUT_FILENO));
        ASSERT_SYS_OK(dup2(errFd[1], STDERR_FILENO));
        ASSERT_SYS_OK(close(stdFd[0]));
        ASSERT_SYS_OK(close(stdFd[1]));
        ASSERT_SYS_OK(close(errFd[0]));
        ASSERT_SYS_OK(close(errFd[1]));
        ASSERT_SYS_OK(execvp(splitInput[1], splitInput + 1));
    }

    ASSERT_SYS_OK(close(stdFd[1]));
    ASSERT_SYS_OK(close(errFd[1]));

    tasks[taskNumber].stdReadFd = stdFd[0];
    tasks[taskNumber].errReadFd = errFd[0];
    tasks[newTaskNumber].execPid = execPid;

    printf("Task %d started: pid %d.\n", taskNumber, execPid);

    ASSERT_SYS_OK(sem_post(globalMutex));

    int errBufferPid = fork();
    ASSERT_SYS_OK(errBufferPid);

    if (errBufferPid == 0) {
        close(stdFd[0]);
        bufferNextLine(taskNumber, true);
        close(errFd[0]);
        return;
    }

    close(errFd[0]);
    bufferNextLine(taskNumber, false);
    close(stdFd[0]);

    int status;
    waitpid(errBufferPid, 0, 0);
    waitpid(execPid, &status, 0);

    if (WIFEXITED(status)) {
        printf("Task %d ended: status %d.\n", taskNumber, WEXITSTATUS(status));
    } else {
        printf("Task %d ended: signalled.\n", taskNumber);
    }

    for (int i = 0; splitInput[i] != NULL; i++) {
        ASSERT_SYS_OK(munmap(splitInput[i], sizeof(char) * (strlen(splitInput[i]) + 1)));
    }
    ASSERT_SYS_OK(munmap(splitInput, sizeof(char*) * splitInputLength));

}

void createTask(char ** splitInput) {
    newTaskNumber++;

    ASSERT_SYS_OK(sem_init(&tasks[newTaskNumber].mutex, 1, 1));

    tasks[newTaskNumber].lastStdLine = MMAP(sizeof(char) * MAX_TASK_OUT_LENGTH);
    if (tasks[newTaskNumber].lastStdLine == MAP_FAILED) {
        fatal("mmap failed");
    }

    tasks[newTaskNumber].lastErrLine = MMAP(sizeof(char) * MAX_TASK_OUT_LENGTH);
    if (tasks[newTaskNumber].lastErrLine == MAP_FAILED) {
        fatal("mmap failed");
    }

    int splitInputArrSize = 1;

    char ** sharedSplitInput = MMAP(sizeof(char*) * splitInputArrSize);
    if (sharedSplitInput == MAP_FAILED) {
        fatal("mmap failed");
    }

    for (int i = 0; splitInput[i] != NULL; i++) {
        sharedSplitInput[i] = MMAP((strlen(splitInput[i]) + 1) * sizeof(char));
        if (sharedSplitInput[i] == MAP_FAILED) {
            fatal("mmap failed");
        }

        strcpy(sharedSplitInput[i], splitInput[i]);
    }
    sharedSplitInput[splitInputArrSize - 1] = NULL;

    int handlerPid = fork();
    ASSERT_SYS_OK(handlerPid);

    if (handlerPid == 0) {
        runHandler(newTaskNumber, sharedSplitInput, splitInputArrSize);
        _exit(0);
    } else {
        tasks[newTaskNumber].handlerPid = handlerPid;
    }



}

void readTask(int taskNum, bool err) {
    char * line;
    ASSERT_SYS_OK(sem_wait(&tasks[taskNum].mutex));
    if (err) {
        line = tasks[taskNum].lastErrLine;
        printf("Task %d stderr: '%s'.\n", taskNum, line);   
    } else {
        line = tasks[taskNum].lastStdLine;
        printf("Task %d stdout: '%s'.\n", taskNum, line);   
    }

    ASSERT_SYS_OK(sem_post(&tasks[taskNum].mutex));
}

void goSleep(char ** splitInput) {
    usleep(atoi(splitInput[1]) * 1000);
}

void goKill(char ** splitInput) {
    int taskNumber = atoi(splitInput[1]);
    int pid = tasks[taskNumber].execPid;
    kill(pid, SIGINT);
}

int main() {

    setbuf(stdout, 0);
    setbuf(stdin, 0);
    setvbuf(stdout, NULL, _IONBF, 0);

    char * inputLine = malloc(sizeof(char) * MAX_LINE_LENGTH);

    tasks = MMAP(sizeof(Task) * MAX_TASK_N);
    if (tasks == MAP_FAILED) {
        fatal("mmap fail");
    }

    globalMutex = MMAP(sizeof(sem_t));
    if (globalMutex == MAP_FAILED) {
        fatal("mmap fail");
    }

    ASSERT_SYS_OK(sem_init(globalMutex, 1, 1));

    while(true) {
        if(!read_line(inputLine, MAX_LINE_LENGTH, stdin)) {
            break;
        }

        ASSERT_SYS_OK(sem_wait(globalMutex));
        
        char ** splitInput = split_string(inputLine);

        if (splitInput == NULL) {
            fatal("split_string returned null");
        }

        if (strcmp(splitInput[0], "quit") == 0) {
            free_split_string(splitInput); 
            break;
        }
        if (strcmp(splitInput[0], "quit\n") == 0) {
            free_split_string(splitInput); 
            break;
        }

        if (strcmp(splitInput[0], "run") == 0) {
            createTask(splitInput);
        } else {
            if (strcmp(splitInput[0], "out") == 0) {
                readTask(atoi(splitInput[1]), false);
            } else if (strcmp(splitInput[0], "err") == 0) {
                readTask(atoi(splitInput[1]), true);
            } else if (strcmp(splitInput[0], "sleep") == 0) {
                goSleep(splitInput);
            } else if (strcmp(splitInput[0], "kill") == 0) {
                goKill(splitInput);
            }

            ASSERT_SYS_OK(sem_post(globalMutex));
        }

        free_split_string(splitInput);    
    }

    free(inputLine);

    for (int i = 0; i <= newTaskNumber; i++) {
        kill(tasks[i].execPid, SIGKILL);
        waitpid(tasks[i].handlerPid, 0, 0);
        ASSERT_SYS_OK(munmap(tasks[i].lastErrLine, sizeof(char) * MAX_TASK_OUT_LENGTH));
        ASSERT_SYS_OK(munmap(tasks[i].lastStdLine, sizeof(char) * MAX_TASK_OUT_LENGTH));
        ASSERT_SYS_OK(sem_destroy(&tasks[i].mutex));
    }

    ASSERT_SYS_OK(munmap(tasks, sizeof(Task) * MAX_TASK_N));
    ASSERT_SYS_OK(sem_destroy(globalMutex));
    ASSERT_SYS_OK(munmap(globalMutex, sizeof(sem_t)));


    return 0;
}

