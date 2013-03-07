#ifndef _STDLIB_H
#define _STDLIB_H

#include <linux/slab.h>

#define malloc(X) kmalloc(X, GFP_KERNEL)
#define free(X) kfree(X)
#define realloc(X,Y) krealloc(X,Y, GFP_KERNEL)
#define getpid() 0
#define atoi(X) simple_strtol(X, NULL, 10)

// Get environment variable
char * getenv(const char *str);


#endif
