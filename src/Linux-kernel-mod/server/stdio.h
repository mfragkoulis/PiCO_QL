#ifndef _STDIO_H
#define _STDIO_H

#include <linux/kernel.h>

#define printf(...) printk(KERN_ERR __VA_ARGS__)
#define fprintf(X, ...) printk(KERN_ERR __VA_ARGS__)

//fopen(): no-op
#define fopen(X,Y) NULL
 
// Flush data to file
#define fflush(X) (void)X

//fclose(): no-op
#define fclose(X) 0

typedef void * FILE;
extern void *stderr;
extern void *stdout;

#endif
