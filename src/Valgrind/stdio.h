#ifndef _STDIO_H
#define _STDIO_H

#include <string.h>
#include "pub_tool_libcprint.h"

#define printf(...) VG_(umsg)(__VA_ARGS__)
#define fprintf(X, ...) VG_(printf)(__VA_ARGS__)
#define sprintf(...) VG_(sprintf)(__VA_ARGS__)
#define snprintf(...) VG_(snprintf)(__VA_ARGS__)

//fopen(): no-op
#define fopen(X,Y) NULL
 
// Flush data to file
#define fflush(X) (void)X

//fclose(): no-op
#define fclose(X) (void)X

//rename a file
#define rename(X,Y) NULL

typedef void * FILE;
extern void *stderr;
extern void *stdout;

#endif
