#ifndef _NTR_COMLEX_H_
#define _NTR_COMLEX_H_ 1
#include "parse.h"

typedef void (*ComplexFn)(unsigned char **, struct Scene *data, unsigned long *cache_off, unsigned long pos);
extern const ComplexFn comps[PI_MAX-PI_GENERIC];

void generic	(unsigned char **c, struct Scene *data, unsigned long *cache_off, unsigned long pos);
void color	(unsigned char **c, struct Scene *data, unsigned long *cache_off, unsigned long pos);
void fg		(unsigned char **c, struct Scene *data, unsigned long *cache_off, unsigned long pos);
void bg		(unsigned char **c, struct Scene *data, unsigned long *cache_off, unsigned long pos);
void tcolor	(unsigned char **c, struct Scene *data, unsigned long *cache_off, unsigned long pos);
void tfg	(unsigned char **c, struct Scene *data, unsigned long *cache_off, unsigned long pos);
void tbg	(unsigned char **c, struct Scene *data, unsigned long *cache_off, unsigned long pos);

#endif /* ifndef _NTR_COMLEX_H_ */

