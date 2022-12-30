#ifndef _NTR_INPUT_H_
#define _NTR_INPUT_H_ 1
#include <stdbool.h>

typedef const char *InputType;
typedef void *(*InputFn)(char);

#define BIT(NAME) unsigned char NAME:1
typedef union {
	struct {
		BIT(isAsync);
		BIT(isInverted);
	};
	unsigned char bits;
} InputAttr;

// TODO decrease size (from 24 Bytes)
typedef struct {
	// if not null terminated -> set keylen to size
	InputType key;
	InputFn function;
	// when 0 -> until null byte('\0')
	unsigned keylen;
	InputAttr attributes;
} InputHandler;

typedef struct {
	unsigned len;
	InputHandler *handlers;
	InputFn fallbackHandler;
} Input_t;

typedef struct {
	InputType origin;
	unsigned origin_index;
	void *value;
} InputRet;

Input_t inputInit(unsigned len, const InputHandler *handlers, InputFn fallbackHandler);
void inputDestroy(Input_t *restrict input);
void inputLoop(Input_t input, char (*getInput)(void *), void *getInput_arg, InputRet *retVal);

#endif // !_NTR_INPUT_H_

