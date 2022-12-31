#include "input.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "common.h"

Input_t inputInit(unsigned len, const InputHandler handlers[static 1], InputFn fallbackHandler) {
	Input_t ret = {
		.len = len,
		.handlers = malloc(sizeof(InputHandler)*len),
		.fallbackHandler = fallbackHandler
	};
	if(!ret.handlers) // WARNING: Silent fail
		goto err;
	memcpy(ret.handlers, handlers, sizeof(InputHandler)*len);
	return ret;
err:
	return (Input_t){0};
}

void inputDestroy(Input_t input[static 1]) {
	free((void *)input->handlers);
	input->handlers = NULL;
}
// locate character in string up to n bytes or a null byte
static const char *strnchr(const char *restrict s, int c, unsigned n) {
	for(int i = 0; i < n; i++) {
		if(*(s+i) == c)
			return s+i;
		else if(*(s+i) == '\0')
			return NULL;
	}
	return NULL;
}

void inputLoop(Input_t input, char (*getInput)(void *), void *getInput_arg, InputRet *retVal) {
	if(!input.handlers)
		return;
	char c = getInput(getInput_arg);
	if(c == EOF)
		return;
	for(int i = 0; i < input.len; i++) {
		if(input.handlers[i].keylen == 0) { // asume NULL terminated
			if(strchr(input.handlers[i].key, c) != NULL)
				goto aft;
		} else { // read till keylen of NULL byte
			if(strnchr(input.handlers[i].key, c, input.handlers[i].keylen) != NULL)
				goto aft;
		}
		continue;
aft:;
		void *ret = input.handlers[i].function(c);
		// set retVal if not NULL
		if(retVal) {
			*retVal = (InputRet){
				.origin = input.handlers[i].key,
				.origin_index = i,
				.value = ret
			};
		}
		return;
	}
	if(input.fallbackHandler)
		input.fallbackHandler(c);
}

