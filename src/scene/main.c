#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <termios.h>

#include "parse.h"
#include "../term.h"

void inputInit(void);
void inputReset(void);

static inline void clear(void) {
	printf("\033[H\033[J");
}

//int isDebug = 1;

int mainer(int argc, char *argv[]) {
	//inputInit();
	char inp = 'b';
	int running = 1;
	int i = 0;
	struct Scene a = sceneParse("foo.dat");
	/*a.instructions = malloc(256);
	char instr[] = {
		PT_START,
			PT_INSTR_B,
				PI_BLNK_RAPD,
				PI_BOLD,
				PI_ITALIC,
				PI_FRAMED,
			PT_INSTR_E,
			PT_INSTR_B,
				PI_TCOLOR,
				255,0,0,
				0,255,0,
				'h','e','l','l','o',
			PT_INSTR_E,
			PT_INSTR_B,
				PI_RESET,
				PI_NEWLN,
			PT_INSTR_E,
			PT_INSTR_B,
				PI_GENERIC,
				'h','i','\n',
			PT_INSTR_E,
		PT_END
	};
	memcpy(a.instructions, instr, sizeof(instr));
	a.size = sizeof(instr);*/
	/*FILE *f = fopen("foo.dat", "wb");
	fwrite(instr, sizeof(instr), 1, f);
	fclose(f);
	return 0;*/
	struct Scene b = sceneParse("bar.dat");
	while(0) {
		clear();
		if(i == 10000)
			running = 0;
		i++;
		if(i % 2 == 0)
			sceneInterpret(&a);
		else
			sceneInterpret(&b);
		//running = 0;
		//read(STDIN_FILENO, &inp, 1);
		switch (inp) {
			case '\004':
			case 'q':
				running = 0;
				break;
			case 'c':
				clear();
				break;
			case 'o': {
				sceneInterpret(&a);
				fflush(stdout);
				fflush(stderr);
			} break;
			default:
				printf("%c",inp);
				fflush(stdout);
				break;
		}
	}
	fflush(stdout);
	char *string = sceneStringify(a);

	FILE *file = fopen("log.txt", "w");
	if(!file)
		abort();
	fprintf(file, "%s", string);
	fclose(file);
	free(string);

	sceneDestroy(&a);
	sceneDestroy(&b);
	return 0;
}

