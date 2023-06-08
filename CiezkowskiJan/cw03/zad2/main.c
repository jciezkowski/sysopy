#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(int argc, char** argv) {
    if (argc != 2) {
        fprintf(stderr, "expected one argument");
        return 1;
    }
    printf("%s ", argv[0]);
    fflush(stdout);
    execl("/bin/ls", "ls", argv[1], NULL);
}