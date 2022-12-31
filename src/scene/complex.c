#include "complex.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

static char err_msg[72];

extern const char *prims[0x14];

void err(unsigned long pos) {
	FILE *f = fopen("err.txt", "w");
	if(!f)
		goto err_aft;
	fprintf(f, "%lu: %s\n", pos, err_msg);
	fclose(f);
err_aft:;
}

void generic(unsigned char **c, struct Scene *data, unsigned long *cache_off, unsigned long pos) {
	char buff[MAX_STR];
	unsigned i = 0;
	while(*(++*c) != PT_INSTR_E) {
		if(i > MAX_STR) {
			sprintf(err_msg, "PARSE_ERROR:"
				" Text too long(%u)", i);
			err(pos);
		}
		buff[i] = **c;
		i++;
	}
	buff[i] = '\0';
	*cache_off += sprintf(data->cache+*cache_off,
				"%s", buff);
}

void color(unsigned char **c, struct Scene *data, unsigned long *cache_off, unsigned long pos) {
	*cache_off += sprintf(data->cache+*cache_off,
				"\033[38;5;%dm", *(++*c));
	*cache_off += sprintf(data->cache+*cache_off,
				"\033[48;5;%dm", *(++*c));
	char buff[MAX_STR];
	int i = 0;
	while(**c != PT_INSTR_E) {
		if(i > MAX_STR) {
			sprintf(err_msg, "PARSE_ERROR:"
					" Text too long(%u)", i);
			err(pos);
		}
		buff[i] = *(++*c);
		i++;
	}
	buff[i-1] = '\0';
	*cache_off += sprintf(data->cache+*cache_off,
		"%s", buff);
	*cache_off += sprintf(data->cache+*cache_off,
				"%s", prims[PI_RESET]);
	fflush(stdout);
}

void fg(unsigned char **c, struct Scene *data, unsigned long *cache_off, unsigned long pos) {
	*cache_off += sprintf(data->cache+*cache_off,
				"\033[38;5;%dm", *(++*c));
	if(*(++*c) != PT_INSTR_E) {
		puts("PARSE_WARNING: Foreground instruction not closed");
	}
}
void bg(unsigned char **c, struct Scene *data, unsigned long *cache_off, unsigned long pos) {
	*cache_off += sprintf(data->cache+*cache_off,
				"\033[48;5;%dm", *(++*c));
	if(*(++*c) != PT_INSTR_E) {
		puts("PARSE_WARNING: Background instruction not closed");
	}
}

void tcolor(unsigned char **c, struct Scene *data, unsigned long *cache_off, unsigned long pos) {
	// Foreground
	unsigned char r,g,b;
	r = *(++*c);
	g = *(++*c);
	b = *(++*c);
	*cache_off += sprintf(data->cache+*cache_off,
			"\033[38;2;%d;%d;%dm", r,g,b);
	// Background
	r = *(++*c);
	g = *(++*c);
	b = *(++*c);
	*cache_off += sprintf(data->cache+*cache_off,
			"\033[48;2;%d;%d;%dm", r,g,b);
	// Text
	char buff[MAX_STR];
	int i = 0;
	while(**c != PT_INSTR_E) {
		//printf("%02x\n", *c);
		if(i > MAX_STR) {
			sprintf(err_msg, "PARSE_ERROR:"
				" Text too long(%u)", i);
			err(pos);
		}
		buff[i] = *(++*c);
		i++;
	}
	buff[i-1] = '\0';
	*cache_off += sprintf(data->cache+*cache_off,
			"%s", buff);
	*cache_off += sprintf(data->cache+*cache_off,
			"\033[39\033[49");
	fflush(stdout);
}
void tfg(unsigned char **c, struct Scene *data, unsigned long *cache_off, unsigned long pos) {
	char r,g,b;
	r = *(++*c);
	g = *(++*c);
	b = *(++*c);
	*cache_off += sprintf(data->cache+*cache_off,
			"\033[38;2;%d;%d;%dm", r,g,b);
	if(*(++*c) != PT_INSTR_E) {
		strcpy(err_msg, "PARSE_WARNING:"
				"Foreground instruction not closed");
	}
}
void tbg(unsigned char **c, struct Scene *data, unsigned long *cache_off, unsigned long pos) {
	char r,g,b;
	r = *(++*c);
	g = *(++*c);
	b = *(++*c);
	*cache_off += sprintf(data->cache+*cache_off,
			"\033[48;2;%d;%d;%dm", r,g,b);
	if(*(++*c) != PT_INSTR_E) {
		strcpy(err_msg, "PARSE_WARNING:"
			"Background instruction not closed");
	}
}

