#include "grid.h"
#include <stdlib.h>
#include <time.h>
#include <ncurses.h>
#include <pthread.h>
#include <signal.h>
#include <unistd.h>

const int grid_width = 30;
const int grid_height = 30;
pthread_t* threads;

char *create_grid()
{
    return malloc(sizeof(char) * grid_width * grid_height);
}

void destroy_grid(char *grid)
{
    free(grid);
}

void draw_grid(char *grid)
{
    for (int i = 0; i < grid_height; ++i)
    {
        // Two characters for more uniform spaces (vertical vs horizontal)
        for (int j = 0; j < grid_width; ++j)
        {
            if (grid[i * grid_width + j])
            {
                mvprintw(i, j * 2, "■");
                mvprintw(i, j * 2 + 1, " ");
            }
            else
            {
                mvprintw(i, j * 2, " ");
                mvprintw(i, j * 2 + 1, " ");
            }
        }
    }

    refresh();
}

void init_grid(char *src, char *dst)
{
    threads = malloc(sizeof(pthread_t) * grid_height * grid_width);
    for (int i = 0; i < grid_width * grid_height; ++i) {
        src[i] = rand() % 2 == 0;
        arguments* arg = malloc(sizeof(arguments));
        arg->cell = i;
        arg->src = src;
        arg->dst = dst;
        pthread_create(&threads[i], NULL, (void*)start_thread, arg);
    }
}

void* start_thread(void* arg) {
    signal(SIGUSR1, sigusr_handler);
    arguments* args = (arguments*) arg;
    char* tmp;
    int row = args->cell / grid_width;
    int column = args->cell % grid_width;
    int cell = args->cell;
    char* src = args->src;
    char* dst = args->dst;
    free(arg);
    while(1) {
        dst[cell] = is_alive(row, column, src);
        
        pause();

        tmp = src;
        src = dst;
        dst = tmp;
    }
}

void sigint_handler(int signo) {
    free(threads);
    free_memory();
}

void sigusr_handler(int signo) {}

bool is_alive(int row, int col, char *grid)
{

    int count = 0;
    for (int i = -1; i <= 1; i++)
    {
        for (int j = -1; j <= 1; j++)
        {
            if (i == 0 && j == 0)
            {
                continue;
            }
            int r = row + i;
            int c = col + j;
            if (r < 0 || r >= grid_height || c < 0 || c >= grid_width)
            {
                continue;
            }
            if (grid[grid_width * r + c])
            {
                count++;
            }
        }
    }

    if (grid[row * grid_width + col])
    {
        if (count == 2 || count == 3)
            return true;
        else
            return false;
    }
    else
    {
        if (count == 3)
            return true;
        else
            return false;
    }
}

void update_grid(char *src, char *dst)
{
    for (int i = 0; i < grid_height * grid_width; ++i) pthread_kill(threads[i], SIGUSR1);
}