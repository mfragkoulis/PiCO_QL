#ifndef _STDLIB_H
#define _STDLIB_H

#include <linux/slab.h>

//#define malloc(X) kmalloc(X, GFP_KERNEL)
#define malloc(X) instr_malloc(X)
//#define free(X) kfree(X)
#define free(X) instr_free(X)
//#define realloc(X,Y) krealloc(X,Y, GFP_KERNEL)
#define realloc(X,Y) instr_realloc(X,Y)
#define getpid() 0
#define atoi(X) simple_strtol(X, NULL, 10)

extern unsigned long memMaxFootprint;
extern unsigned long memFootprint;
void * instr_malloc(size_t size);
void instr_free(void *ptr);
void * instr_realloc(void *ptr, size_t size);

// Get environment variable
char * getenv(const char *str);


#endif
