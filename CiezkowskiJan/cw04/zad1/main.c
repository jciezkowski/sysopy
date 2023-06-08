#define _XOPEN_SOURCE 700

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <signal.h>
#define TEST_SIGNAL SIGUSR1

void signal_handler(int signum) {
    printf("received signal %d\n", signum);
}

int handle_argument(char* argument) {
    sigset_t mask;
    sigemptyset(&mask);
    sigaddset(&mask, TEST_SIGNAL);
    if (strcmp(argument, "ignore") == 0) {
        signal(TEST_SIGNAL, SIG_IGN);
    }
    else if (strcmp(argument, "handler") == 0) {
        signal(TEST_SIGNAL, signal_handler);
    }
    else if (strcmp(argument, "mask") == 0) {
        signal(TEST_SIGNAL, signal_handler);
        if (sigprocmask(SIG_BLOCK, &mask, NULL) < 0) {
            fprintf(stderr, "could not block signal\n");
        }
    }
    else if (strcmp(argument, "pending") == 0) {
        signal(TEST_SIGNAL, signal_handler);
        if (sigprocmask(SIG_BLOCK, &mask, NULL) < 0) {
            fprintf(stderr, "could not block signal\n");
        }
    }
    else {
        fprintf(stderr, "invalid argument value\n");
        return 0;
    }
    raise(TEST_SIGNAL);
    return 1;
}

void test_pending() {
    sigset_t set;
    sigpending(&set);
    if (sigismember(&set, TEST_SIGNAL)) printf("signal pending\n");
    else printf("signal not pending\n");
}

int main(int argc, char** argv) {
    // weryfikacja liczby argumentow
    if (argc != 3) {
        fprintf(stderr, "expected 2 arguments\n");
        return 1;
    }
    // obsluga polecen
    if (strcmp(argv[2], "fork") == 0) {
        int executed = handle_argument(argv[1]);
        pid_t pid = fork();
        if (pid == 0) {
            if (strcmp(argv[1], "pending") == 0) {
                test_pending();
            }
            else if (executed == 1) {
                raise(TEST_SIGNAL);
            }
        }
    }
    else if (strcmp(argv[2], "exec") == 0) {
        if (strcmp(argv[1], "handler") == 0) {
            fprintf(stderr, "invalid argument for exec option\n");
            return 2;
        }
        else if (handle_argument(argv[1]) == 1) execl(argv[0], argv[0], argv[1], "child", NULL);
    }
    else if (strcmp(argv[2], "child") == 0) {
        if (strcmp(argv[1], "pending") == 0) test_pending();
        else raise(TEST_SIGNAL);
    }
}