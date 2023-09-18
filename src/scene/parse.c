#include "parse.h"
#include "defs.h"
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <ctype.h>

#include "complex.h"

#define ESC "\033["

// TODO: Should it be read from file?
static const char *sceneLut[] = {
	"NewLine", "Reset", "BlinkSlow", "BlinkRapid", "BlinkStop", "Bold", "Italic", "ItalicEnd",
	"Underline", "DoubleUnderline", "UnderlineEnd", "Faint",
	"Strike", "StrikeEnd", "NormalIntents", "Framed", "Circled", "NoFrame", "Overline", "OverlineEnd",
	"GenericText", "Color", "Foreground", "Background", "TrueColor", "TrueForeground", "TrueBackground"
};

// accessed in complex.c
const char *prims[0x14] = {
	"\n",
	ESC"0m",
	// Blinking
	ESC"5m", ESC"6m", ESC"25m",
	// Bold
	ESC"1m",
	// Italic
	ESC"3m", ESC"23m",
	// Underline/DoubleUnderline
	ESC"4m", ESC"21m", ESC"24m",
	// Faint
	ESC"2m",
	// Strike
	ESC"9m", ESC"29m",
	// Normal intensity
	ESC"22m",
	// Frame, Circled
	ESC"51m", ESC"52m", ESC"54m",
	// Overline
	ESC"53m", ESC"55m"
};
/*{
	if(isDebug) {
		printf("%lu\tAdded:\t\t%02x", pos, c);
		if(ret.instructions[pos-1] == PT_INSTR_B) {
       		printf("(%s)", sceneLut[c]);
    	}
        puts("");
	}
	ret.instructions[pos] = c;
	pos++;
}*/
#define INSTR_ADD(c) { if(isDebug) { printf("%lu\tAdded:\t\t%02x", pos, c); if(ret.instructions[pos-1] == PT_INSTR_B) { printf("(%s)", sceneLut[c]); } puts("");}ret.instructions[pos] = c; pos++;}

#define INSTR_ADD_NO_NL(c) \
{ \
	if(isDebug) { printf("%lu\tAdded:\t\t%02x", pos, c); }\
	ret.instructions[pos] = c; \
	pos++; \
}
// set C to next c(either from file or from buff)
#define C_NEXT(C) {if(i >= 3) { C = getc(file); } else { C = buff[i+1]; }; i++; }

const ComplexFn comps[PI_MAX-PI_GENERIC] = {
	generic, color, fg, bg, tcolor, tfg, tbg
};

extern const bool isDebug;

struct Scene sceneParse(const char *restrict fname) {
	struct Scene ret;
	ret.size = DEF_FD_LEN;
	ret.cache = NULL;
	ret.instructions = malloc(sizeof(int) * DEF_FD_LEN);
	if(!ret.instructions) {
		fprintf(stderr, "ERROR: Couldn't allocate for Scene in parseFile\n");
		ret.size = 0;
		return ret;
	}
	FILE *file = fopen(fname, "rb");
	if(!file) {
		free(ret.instructions);
		ret.instructions = NULL;
		ret.size = 0;
		fprintf(stderr, "ERROR: Couldn't open file: %s\n", fname);
		return ret;
	}
	int c;
	while((c = getc(file)) != PT_START && c != EOF) {}
	if(c == EOF)
		abort();
	unsigned long pos = 0;
	INSTR_ADD(PT_START);
	char buff[4] = "\0\0\0\0";

	while(fread(buff, 4, 1, file) != -1) {
		for(unsigned char i = 0, c = *buff; i < 4; i++, c = buff[i]) {
			while(c == 0x00)
				C_NEXT(c);
			if(c == PT_END) {
				INSTR_ADD(c);
				goto after;
			}
			if(pos >= ret.size) {
				if(isDebug)
					puts("NOTE: Resizing");
				if((pos * 2) > MAX_FD_LEN) {
					fprintf(stderr, "ERROR: File too long\n");
					if(isDebug) {
						printf("DEBUG: pos=%lu, MAX_FD_LEN=%d\n", pos, MAX_FD_LEN);
					}
					ret.size = pos;
					return ret;
				}
				ret.instructions = realloc(ret.instructions, sizeof(ret.instructions) * 2);
				if(!ret.instructions) {
					fprintf(stderr, "ERROR: Couldn't reallocate Scene.instructions\n");
					return ret;
				}
				ret.size *= 2;
			}
			if(c == PT_INSTR_B) {
				INSTR_ADD(c);
				C_NEXT(c);
				if(c == PI_TCOLOR) {
					INSTR_ADD(c);
					for(int j = 0; j < 6; j++) {
						C_NEXT(c);
						INSTR_ADD(c);
					}
				} else {
					INSTR_ADD(c);
				}
				int depth = 0;
				// Add all bytes until PT_INSTR_E
				while(c != PT_INSTR_E && depth < MAX_INTR_DEPTH) { // primitive list
					depth++;
					C_NEXT(c);
					INSTR_ADD_NO_NL(c);
					if(isDebug) {
						if(isalpha(c))
							printf("(%c)", c);
						puts("");
					}
				}
				continue;
			}
			INSTR_ADD(c);
		}
		buff[0] = '\0';
		buff[1] = '\0';
		buff[2] = '\0';
		buff[3] = '\0';
	}
after:
	ret.size = pos;
	fclose(file);
//	for(int i = 0; i < ret.size; i++)
//		printf("f %02x\n", ret.instructions[i]);
	return ret;
}

void sceneCompile(struct Scene *data) {
	if(data->instructions == NULL)
		return;
	if(data->cache != NULL) { // Already cached
		printf("%s", data->cache);
		return;
	} else { // First time
		data->cache = malloc(CACHE_SZ);
		if(!data->cache) {
			fprintf(stderr, "ERROR: Couldn't allocate for cache\n");
			exit(-2);
		}
		strcpy(data->cache, "");
	}
	unsigned char *c = data->instructions;
	unsigned long pos = 0;
	char err_msg[74];
	// Cache offset
	unsigned long cache_off = 0;
	int started = 0;
	for(unsigned long i = 0; i < data->size; i++) {
		if(!started) {
			if(*c == PT_START)
				started = 1;
			goto cont;
		}
		switch (*c) {
			case PT_END:
				return;
			case PT_INSTR_B: {
				c++;
				//printf("here %02x\n", *c);
				if(*c == PT_INSTR_E)
					goto cont;
				if(*c < PI_GENERIC) { // c is primitive, next must be a primitive or 0xFF
prim_up:
					//printf("\n%02x\n", *c);
					cache_off += sprintf(data->cache+cache_off, "%s", prims[*c]);
					c++;
					if(*c < PI_GENERIC) { // primitive list
						goto prim_up;
					} else if(*c == PT_INSTR_E) {
						continue;
					} else if(*c >= PI_GENERIC) {
						sprintf(err_msg, "PARSE_ERROR:"
								" Complex instruction(%02x) after primitive is illegal", *c);
						goto err;
					} else {
						sprintf(err_msg, "PARSE_ERROR:"
							"'%02x' instruction not closed", *c);
						goto err;
					}
				} else {
					comps[*c-PI_GENERIC](&c, data, &cache_off, pos);
				}
			}
		}
cont:
		c++;
		pos++;
	}
err:;
	FILE *f = fopen("err.txt", "w");
	if(!f)
		goto err_aft;
	fprintf(f, "%lu: %s\n", pos, err_msg);
	fclose(f);
err_aft:;
	fprintf(stderr, "%lu: %s\n", pos, err_msg);
	exit(-1);
}

void sceneInterpret(struct Scene *data) {
	sceneCompile(data);
    	printf("%s", data->cache);
	fflush(stdout);
	return;
}

static void *interp_mediator(void *data) {
	sceneInterpret((struct Scene *)data);
	return NULL;
}
// Interpret struct Scene, print to stdout
pthread_t sceneInterpret_async(struct Scene *data) {
	pthread_t ret;
	if(pthread_create(&ret, NULL, interp_mediator, data) != 0) {
		FILE *f = fopen("err.txt", "w");
		if(!f) {
			fprintf(stderr, "ERROR: Couldn't create pthread_t\n");
			exit(-1);
		}
		fprintf(f, "ERROR: Couldn't create pthread_t\n");
		fclose(f);
	}
	return ret;
}

void sceneDestroy(struct Scene *data) {
	if(data->instructions) {
		free(data->instructions);
		data->instructions = NULL;
	}
	if(data->cache) {
		free(data->cache);
		data->cache = NULL;
	}
}

// WARNING: return value is dynamically allocated
// Assumes valid scene
char *sceneStringify(struct Scene scene) {
	char *dest = malloc(128);
	strncpy(dest, 
		"Scene: {\n\t",
		128);
	for(int i = 0; i < scene.size; i++) {
		const unsigned char *inst = &*scene.instructions;
		if(inst[i] == PT_INSTR_B) {
			i++;
			strcat(dest, sceneLut[inst[i]]);
			strcat(dest, " ");
			if(inst[i] < PI_GENERIC) { // primitive
				printf("\n\t");
			} else { // complex 
				
				if(inst[i] == PI_GENERIC) { // TODO: corrupted texr
					strcat(dest, sceneLut[PI_GENERIC]);
					int j = 0;
					char buff[MAX_STR];
					do {
						i++; j++;
						buff[j] = inst[i];
					} while(inst[i] != PT_INSTR_E) ;
					buff[j] = '\0';
					strcat(dest, buff);
					strcat(dest, " ");
				} else { 
					if(inst[i] == PI_FG || inst[i] == PI_BG) {
						i++;
						strncat(dest, (char *)&inst[i], 1);
						i += 2; // skip ST_INSTR_E too
					} else if(inst[i] == PI_TFG || inst[i] == PI_TBG) {
						// TODO
					}
					// TODO: other complex instructions
				}
			}
		}
	}
	strcat(dest, "}");
	return dest;
}

void scenePrint(struct Scene scene) {
	// TODO
}

bool isSceneValid(struct Scene scene) {
	// TODO
	return true;
}

