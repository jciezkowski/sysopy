#define _XOPEN_SOURCE 700

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <signal.h>
#include <ctype.h>

#define TEST_SIGNAL SIGUSR1

int isnumber(char* argument, int len) {
    int i = 0;
    while (i < len) {
        if (isdigit(argument[i]) == 0) return 0;
        i++;
    }
    return 1;
}

void signal_handler(int signo) {
    printf("received signal from catcher\n");
}

int main(int argc, char** argv) {
    if (argc < 3) {
        fprintf(stderr, "invalid number of arguments\n");
    }
    pid_t pid;
    if (isnumber(argv[1], strlen(argv[1]))) pid = atoi(argv[1]);
    else {
        fprintf(stderr, "invalid PID\n");
        return 1;
    }
    struct sigaction act;
    sigset_t mask;
    sigset_t empty_mask;
    sigemptyset(&mask);
    sigaddset(&mask, TEST_SIGNAL);
    act.sa_handler = signal_handler;
    sigaction(TEST_SIGNAL, &act, NULL);
    for (int i = 2; i < argc; i++) {
        if (strlen(argv[i]) == 1 && isdigit(argv[i][0])){
            union sigval val;
            val.sival_int = atoi(argv[i]);
            if(sigqueue(pid, TEST_SIGNAL, val) == -1){
                fprintf(stderr, "invalid PID number\n");
                return 1;
            };
            sigsuspend(&empty_mask);
        }
    }
    return 0;
}