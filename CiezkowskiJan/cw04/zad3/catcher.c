#define _XOPEN_SOURCE 700

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <signal.h>
#include <ctype.h>
#include <sys/times.h>
#include <time.h>

#define TEST_SIGNAL SIGUSR1

int counter = 0;
int mode = 0;

clock_t get_time() {
    struct tms ts_tms;
    clock_t current_time = times(&ts_tms);
    return current_time;
}

void signal_handler(int signum, siginfo_t* info, void* context) {
    mode = info->si_value.sival_int;
    counter++;
    pid_t pid = info->si_pid;
    kill(pid, TEST_SIGNAL);
}

int main() {
    printf("PID procesu: %d\n\n", (int)getpid());
    sigset_t mask;
    sigemptyset(&mask);
    struct sigaction act;
    act.sa_mask = mask;
    act.sa_flags = SA_SIGINFO;
    act.sa_sigaction = signal_handler;
    sigaction(SIGUSR1, &act, NULL);
    time_t current_time;
    clock_t interval;
    while(1){
        switch(mode) {
            case 0:
                continue;
            case 1:
                for (int i = 0; i < 101; i++) printf("%d ", i);
                printf("\n\n");
                mode = 0;
                break;
            case 2:
                time(&current_time);
                printf("%s\n", ctime(&current_time));
                mode = 0;
                break;
            case 3:
                printf("number of requests: %d\n\n", counter);
                mode = 0;
                break;
            case 4:
                time(&current_time);
                printf("%s\n", ctime(&current_time));
                interval = get_time();
                while (mode == 4) {
                    if ((get_time() - interval)/100 >= 1) {
                        time(&current_time);
                        printf("%s\n", ctime(&current_time));
                        interval = get_time();
                    }
                }
                break;
            case 5:
                exit(0);
                break;
            default:
                fprintf(stderr, "invalid code\n\n");
                mode = 0;
                break;
        }
    }
}