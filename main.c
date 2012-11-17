#include <stdio.h>
#include <stdlib.h>
#include <ncurses.h>
#include <time.h>
#include <unistd.h>

/*
 * TODO: game probably should be a struct, with width and height
 */

#define SLEEP_TIME 200000  //The time to sleep between updates, in microseconds

char** make_game(int width, int height);
void copy_game(char **to, char **from, int width, int height);
int alive_neighbours(char **game, int x, int y, int width, int height);

int main(int argc, char **argv)
{
	int max_x, max_y;
	int x, y;
	char **prev_game;
	char **game;

	srand(time(NULL));
	initscr();
	getmaxyx(stdscr, max_y, max_x);
	game = make_game(max_x, max_y);
	prev_game = make_game(max_x, max_y); //Because the malloc is inside the function. Change this
	copy_game(prev_game, game, max_x, max_y);
	for (;;) {
		move(0, 0);
		for (y = 0; y < max_y; ++y) {
			for (x = 0; x < max_x; ++x) {
				addch(game[y][x]);
			}
		}
		for (y = 0; y < max_y; ++y) {
			for (x = 0; x < max_x; ++x) {
				int alive = alive_neighbours(prev_game, x, y, max_x, max_y);
				/*if (prev_game[y][x] == '#') { //could be more readable
					if ((alive < 2) || (alive > 3)) {
						game[y][x] = '.';
					}
				}
				else if (alive == 3) {
					game[y][x] = '#';
				}*/
				if ((alive < 2) || (alive > 3)) {
					game[y][x] = '.';
				}
				else if ((alive == 3) || (alive == 2 && game[y][x] == '#')){
					game[y][x] = '#';
				}
			}
		}
		copy_game(prev_game, game, max_x, max_y);
		refresh();
		usleep(SLEEP_TIME);
	}
	endwin();
	return 0;
}

char** make_game(int w, int h)
{
	char **game;
	int x, y;
	game = malloc(sizeof(char*) * h);
	for (x = 0; x < w; ++x) {
		game[x] = malloc(sizeof(char) * w);
	}
	for (y = 0; y < h; ++y) {
		for (x = 0; x < w; ++x) {
			if ((rand() % 10) > 8) { //Magic number
				game[y][x] = '#';
			}
			else {
				game[y][x] = '.';
			}
		}
	}
	return game;
}

void copy_game(char **to, char **from, int w, int h)
{
	int x, y;
	for (y = 0; y < h; ++y) {
		for (x = 0; x < w; ++x) {
			to[y][x] = from[y][x];
		}
	}
}

int alive_neighbours(char **game, int x, int y, int w, int h)
{
	int alive = 0;
	int x_offs, y_offs;
	for (y_offs = -1; y_offs <= 1; ++y_offs) {
		for (x_offs = -1; x_offs <= 1; ++x_offs) {
			if ((x + x_offs >= 0) && (x + x_offs < w) &&
				(y + y_offs >= 0) && (y + y_offs < h))
			{
					fprintf(stderr, "%d + %d, %d + %d is checked\n", y, y_offs, x, x_offs);
				if ((x == 0) && (y == 0)) {
					continue;
				}
				if (game[y + y_offs][x + x_offs] == '#') {
					++alive;
				}
			}
		}
	}
	return alive;
}
