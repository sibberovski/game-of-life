#include <stdio.h>
#include <stdlib.h>
#include <ncurses.h>
#include <time.h>
#include <unistd.h>
#include <string.h>

#define DEAD_PERCENTAGE 93 //Decides the chance of a cell being dead when initialising the game.
#define SLEEP_TIME 200000  //The time to sleep between updates, in microseconds
#define ALIVE_CELL '#'
#define DEAD_CELL ' '

typedef struct game {
	char **board;
	int width;
	int height;
} game;

void game_alloc(game *g, int width, int height);
void init_game(game *g);
void copy_game(game *to, game *from);
int alive_neighbours(game *g, int x, int y); //needs testing, hopefully the error is here
void cleanup(int error);

int main(int argc, char **argv)
{
	int height, width;
	int x, y;
	game current_game;
	game prev_game;
	srand(time(NULL));
	initscr();
	getmaxyx(stdscr, height, width);
	game_alloc(&current_game, width, height);
	game_alloc(&prev_game, width, height);
	init_game(&current_game);
	copy_game(&prev_game, &current_game);
	for (;;) {
		move(0, 0);
		for (y = 0; y < current_game.height; ++y) {
			for (x = 0; x < current_game.width; ++x) {
				addch(current_game.board[y][x]);
			}
		}
		for (y = 0; y < current_game.height; ++y) {
			for (x = 0; x < current_game.width; ++x) {
				int alive = alive_neighbours(&current_game, x, y);
				if ((alive < 2) || (alive > 3)) {
					current_game.board[y][x] = DEAD_CELL;
				}
				else if ((alive == 3) || (alive == 2 && current_game.board[y][x] == ALIVE_CELL)) {
					current_game.board[y][x] = ALIVE_CELL;
				}
			}
		}
		copy_game(&prev_game, &current_game);
		refresh();
		usleep(SLEEP_TIME);
	}
	cleanup(0);
}

void game_alloc(game* g, int width, int height)
{
	int i;
	g->width = width;
	g->height = height;
	g->board = malloc(sizeof(char*) * height);
	for (i = 0; i < width; ++i) {
		g->board[i] = malloc(sizeof(char) * width);
	}
}

void init_game(game *g)
{
	int x, y;
	for (y = 0; y < g->height; ++y) {
		for (x = 0; x < g->width; ++x) {
			if ((rand() % 100) > DEAD_PERCENTAGE) { //Not very statistically correct
				g->board[y][x] = ALIVE_CELL;
			}
			else {
				g->board[y][x] = DEAD_CELL;
			}
		}
	}
}

void copy_game(game *to, game *from)
{
	if ((to->width != from->width) || (to->height != from->height)) {
		fprintf(stderr, "copy_game handed games of different sizes. Exiting\n");
		cleanup(1);
	}
	int i;
	for (i = 0; i < from->height; ++i) {
		memcpy(to->board[i], from->board[i], from->width);
	}
}

int alive_neighbours(game *g, int x, int y)
{
	int alive = 0;
	int x_offs, y_offs;
	for (y_offs = -1; y_offs <= 1; ++y_offs) {
		for (x_offs = -1; x_offs <= 1; ++x_offs) {
			if ((x + x_offs >= 0) && (x + x_offs < g->width) &&
				(y + y_offs >= 0) && (y + y_offs < g->height))
			{
				if ((x_offs == 0) && (y_offs == 0)) {
					continue;
				}
				if (g->board[y + y_offs][x + x_offs] == ALIVE_CELL) {
					++alive;
				}
			}
		}
	}
	return alive;
}

void cleanup(int error)
{
	endwin();
	exit(error);
}
