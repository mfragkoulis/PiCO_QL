#ifndef _TIME_H
#define _TIME_H

#include <linux/time.h>
#include <linux/kernel.h>

/* Like touch to update last access timestamp (?) 
 * - deprecated. Actually defined in sys/time.h */
#define utimes(X,Y) (void)X

#define time(X) (*X)=CURRENT_TIME_SEC.tv_sec
/* Not needed when compiling with 
 * SQLITE_OMIT_FLOATING_POINT
 */
int gettimeofday(struct timeval *t, void *tzp);

struct tm * gmtime(const time_t * t);
struct tm * localtime(const time_t * t);

/* Actually defined in sys/time.h */
size_t strftime(char *buf, size_t size, 
                const char *fmt, 
                const struct tm *time_tm);

#endif
