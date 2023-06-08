#pragma once
#include <stdbool.h>

typedef struct arguments {
    int cell;
    char* src;
    char* dst;
} arguments;

char *create_grid();
void destroy_grid(char *grid);
void draw_grid(char *grid);
void init_grid(char *src, char *dst);
bool is_alive(int row, int col, char *grid);
void update_grid(char *src, char *dst);
void* start_thread(void* arg);
void sigint_handler(int signo);
void sigusr_handler(int signo);
void free_memory();