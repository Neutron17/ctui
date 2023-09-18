#ifndef _NTR_PARSE_H_
#define _NTR_PARSE_H_ 1

#include "defs.h"
#include <pthread.h>
#include <stdbool.h>

enum SepEnc {
	SE_SCOLON,
	SE_NLN,
};
enum InstrEnc {
	// instr instr instr
	ES_SPACE,
	ES_NLN,
	ES_COMMA_COLON,
	ES_SCOLON_COLON
};

struct Scene {
	// Dynamically allocated list of instruction bytes
	unsigned char *instructions;
	// Length of instructions
	unsigned size;
	// Cached output
	char *cache;
};

/**************
*  FUNCIONS  *
**************/

/* How to use?
 *  1. Call sceneParse(file)/sceneInit(array of data), save the return value to a struct Scene
 *  2. Call sceneInterpret or sceneInterpret_async
 */

// Read file into a struct Scene
struct Scene sceneParse(const char *fname);
void sceneCompile(struct Scene *data);

// Initialize scene with instrs, char array of size len
struct Scene sceneInit(const char *instrs, size_t len); 

// Interpret struct Scene, print to stdout
void sceneInterpret(struct Scene *data);
// Interpret struct Scene, print to stdout
pthread_t sceneInterpret_async(struct Scene *data);

// frees the fields of argument data and makes them point to NULL
void sceneDestroy(struct Scene *data);

// WARNING: return value is dynamically allocated
char *sceneStringify(struct Scene scene);
void scenePrint(struct Scene scene);

bool isSceeneValid(struct Scene scene);

#endif

