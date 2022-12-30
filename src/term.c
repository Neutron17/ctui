#include "term.h"
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <termios.h>

static struct termios defattr;

void termiosInit(void) {
	if(!isatty(STDIN_FILENO)) {
		fprintf(stderr, "ERROR: Not a terminal\n");
		exit(EXIT_FAILURE);
	}
	tcgetattr(STDIN_FILENO, &defattr);
	atexit(termiosReset);
	struct termios tattr;

	tcgetattr(STDIN_FILENO, &tattr);
	tattr.c_lflag	&= ~(ICANON | ECHO);
	tattr.c_cc[VMIN] = 1;
	tattr.c_cc[VTIME]= 0;
	tcsetattr(STDIN_FILENO, TCSAFLUSH, &tattr);
}

void termiosReset(void) {
	tcsetattr(STDIN_FILENO, TCSANOW, &defattr);
}


