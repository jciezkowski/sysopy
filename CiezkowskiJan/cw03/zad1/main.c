#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
#include <ctype.h>

int is_number(const char* arg) {
    for (int i = 0; i < strlen(arg); i++) {
        if (!(isdigit(arg[i]))) {
            return 0;
        }
    }
    return 1;
}

int main(int argc, char** argv) {
    if (argc != 2) {
        fprintf(stderr, "expected one argument");
        return 1;
    }
    if (!(is_number(argv[1]))) {
        fprintf(stderr, "argument is not a number");
        return 2;
    }
    int no_processes = atoi(argv[1]);
    pid_t pid;
    for (int i = 0; i < no_processes; i++) {
        pid = fork();
        if (pid == 0) {
            printf("PID procesu macierzystego: %d, PID procesu potomnego: %d\n", (int)getppid(), (int)getpid());
            return 0;
        }
        else wait(NULL);
    }
    printf("Liczba procesów: %s\n", argv[1]);
    return 0;
}