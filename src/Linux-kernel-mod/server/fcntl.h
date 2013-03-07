#ifndef _FCNTL_H
#define _FCNTL_H

#include <linux/fcntl.h>

// Dummy macro placeholder
//#define fcntl(X,Y,Z) (X += 3)
static int fcntl(int fd, int cmd, long arg){return 0;};

#endif
