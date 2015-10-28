all:2048.c ncurses.h pdcurses.a
	gcc -Wall -std=gnu11 2048.c -o 2048 -lcurses