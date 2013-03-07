#ifndef _STAT_H
#define _STAT_H

#include <linux/stat.h>

static int mkdir(const char *path, int mode){return 0;};

static mode_t umask(mode_t mask){return 0;};

#endif
