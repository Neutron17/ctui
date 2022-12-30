#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <termios.h>
#include <stdbool.h>

#include "term.h"
#include "input.h"

#define ESC "\033"

/*
 * VARIABLES
 */

bool running = true;

/*
 * FUNCTIONS
 */

static inline void clear(void) {
	printf(ESC"[H"ESC"[J");
}

void *genericHandler(char c) {
	printf("%c", c);
	return NULL;
}
void *quit(char c) {
	running = false;
	return NULL;
}
void *move(char c) {
	printf("Moved: %c", c);

	return NULL;
}

char inputGetter(void *arg) {
	char ret;
	read(STDIN_FILENO, &ret, 1);
	return ret;
}

int main(int argc, char *argv[]) {
	termiosInit();
	clear();
	InputHandler handlers[2] = {
		(InputHandler) {
			.key = "wasd",
			.function = move,
			.attributes = {	.bits = 0 },
		},
		(InputHandler) {
			.key = "q",
			.function = quit,
			.attributes = {.bits = 0}
		}
	};
	Input_t inp = inputInit(
		2,
		&*handlers,
		genericHandler
	);

	while(running) {
		clear();
		inputLoop(inp, inputGetter, NULL, NULL);
		fflush(stdout);
	}
	inputDestroy(&inp);
	return 0;
}

