#ifndef _STRING_H
#define _STRING_H

#include <linux/string.h>

#define strtol(X,Y,Z) simple_strtol(X,Y,Z)

char *strerror(int errnum);

#endif
