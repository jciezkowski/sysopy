#include <ncurses.h>
#include <locale.h>
#include <unistd.h>
#include <stdbool.h>
#include "grid.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <pthread.h>
#include <signal.h>
#include <ctype.h>
#include <string.h>

char *foreground, *background;
int work = 1;

int is_number(const char* arg) {
    for (int i = 0; i < strlen(arg); i++) {
        if (!(isdigit(arg[i]))) {
            return 0;
        }
    }
    return 1;
}

int main(int argc, char** argv)
{
	if (argc != 2) {
		fprintf(stderr, "invalid number of arguments\n");
		return 1;
	}
	if (!(is_number(argv[1]))) {
		fprintf(stderr, "argument must be an integer\n");
		return 2;
	}
	int no_threads = atoi(argv[1]);
	srand(time(NULL));
	setlocale(LC_CTYPE, "");
	initscr(); // Start curses mode

	foreground = create_grid();
	background = create_grid();
	char *tmp;

	init_grid(foreground, background, no_threads);
	signal(SIGINT, sigint_handler);
	while (work)
	{
		draw_grid(foreground);
		usleep(500 * 1000);
		if (work) {
			// Step simulation
			update_grid(foreground, background);
			tmp = foreground;
			foreground = background;
			background = tmp;
		}
	}
	endwin(); // End curses mode
	return 0;
}

void free_memory() {
	free(foreground);
	free(background);
	work = 0;
}
