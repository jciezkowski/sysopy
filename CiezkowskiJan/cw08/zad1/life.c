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

char *foreground, *background;
int work = 1;

int main()
{
	srand(time(NULL));
	setlocale(LC_CTYPE, "");
	initscr(); // Start curses mode

	foreground = create_grid();
	background = create_grid();
	char *tmp;

	init_grid(foreground, background);
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
