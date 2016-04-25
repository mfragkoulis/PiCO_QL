#ifndef _STRING_H
#define _STRING_H

#include <stddef.h>
#include "pub_tool_libcbase.h"

#define strcat(X,Y) VG_(strcat)(X,Y)
#define strtol(X,Y,Z) VG_(strtoll10)(X,Y)
#define strcpy(X,Y) VG_(strcpy)(X,Y)     
#define strlen(X) VG_(strlen)(X)
#define strncpy(X,Y,Z) VG_(strncpy)(X,Y,Z)
#define strcmp(X,Y) VG_(strcmp)(X,Y)
#define strncmp(X,Y,Z) VG_(strncmp)(X,Y,Z)
#define strtok(X,Y) VG_(strtok)(X,Y)

#define memcpy(X,Y,Z) VG_(memcpy)(X,Y,Z)
#define memset(X,Y,Z) VG_(memset)(X,Y,Z)
#define memmove(X,Y,Z) VG_(memmove)(X,Y,Z)
#define memcmp(X,Y,Z) VG_(memcmp)(X,Y,Z)

#define random() VG_(random)(0)  // 0 instead of int *pSeed
#define srandomdev() VG_(random)(0)  // __APPLE__

// Left out for compiling on Stereo (mfg - 30/05/2015)
// Call stddef.h for it.
//typedef SizeT size_t;
char *strerror(int errnum);

size_t strlcat(char * restr_dst, const char * restr_src, size_t size);
size_t strlcpy(char * restr_dst, const char * restr_src, size_t size);

#endif
