#ifndef _STDLIB_H
#define _STDLIB_H

#include <string.h>
#include "pub_tool_mallocfree.h"
#include "pub_tool_libcbase.h"
#include "pub_tool_libcproc.h"

#define malloc(X) VG_(malloc)("sqlite malloc",X)
#define free(X) VG_(free)(X)
#define realloc(X,Y) VG_(realloc)("sqlite realloc",X,Y)
#define getpid() 0
#define atoi(X) VG_(strtoll10)(X, NULL)

/* VG: match: Get environment variable */
#define getenv(X) VG_(getenv)(X)

/* VG: match: system() */
#define system(X) VG_(system)(X)

// Pseudo exit()
void exit(int code);

#endif
