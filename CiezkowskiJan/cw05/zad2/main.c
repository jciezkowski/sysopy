#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <ctype.h>
#include <sys/times.h>

clock_t get_time() {
    struct tms ts_tms;
    clock_t current_time = times(&ts_tms);
    return current_time;
}

int is_number(char* argument) {
    int separator_flag = 0;
    if (argument[0] == '.') return 0;
    for (int i = 1; i < strlen(argument); i++) {
        if (argument[i] == '.' && separator_flag == 0) {
            separator_flag = 1;
        } 
        else if (argument[i] == '.' && separator_flag == 1) return 0;
        else if (!(isdigit(argument[i]))) return 0;
    }
    return 1;
}

double f(double x) {
    return 4/(x * x + 1);
}

double integral(double width, int n, int i) {
    double integral_sum = 0;
    double start = i / n;
    double stop = (i + 1) / n;
    int iterations = 0;
    for (double dx = start; dx < stop; dx += width) {
        integral_sum += f(dx);
        iterations++;
    }
    integral_sum *= width;
    double last_dx = (stop - start) - (iterations * width);
    integral_sum += f(stop) * last_dx;
    return integral_sum;
}

int main(int argc, char** argv) {
    if (argc != 3) {
        fprintf(stderr, "invalid number of arguments\n");
        return 1;
    }
    if (!(is_number(argv[1])) || !(is_number(argv[2]))) {
        fprintf(stderr, "invalid argument\n");
    }
    double read_buffer;
    double width = atof(argv[1]);
    int n = atoi(argv[2]);
    double integral_value = 0;
    int fd[n][2];
    clock_t start = get_time();
    for (int i = 0; i < n; i++) {
        pipe(fd[i]);
        if (fork() == 0) {
            close(fd[i][0]);
            double part;
            part = integral(width, n, i);
            write(fd[i][1], &part, sizeof(double));
            close(fd[i][1]);
            exit(0);
        }
    }
    for (int i = 0; i < n; i++) {   
        close(fd[i][1]);
        read(fd[i][0], &read_buffer, sizeof(double));
        integral_value += read_buffer;
        close(fd[i][0]);
    }
    printf("integral value: %f\n", integral_value);
    clock_t stop = get_time();
    printf("execution time: %0.2f s\n", (double)(stop - start) / 100);
    return 0;
}