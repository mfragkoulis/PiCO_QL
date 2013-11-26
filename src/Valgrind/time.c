#include <sys/time.h>
#include <stdio.h>

/* VG:pseudo */ 
int gettimeofday(struct timeval *t, void *tzp) {return 0;};

struct tm * gmtime(const time_t * t) {
  return NULL;
};

struct tm * localtime(const time_t * t) {
  return NULL;
};

// Actually defined in sys/time.h
size_t strftime(char *buf, size_t size, const char *fmt, const struct tm *time_tm) {
  (void)fmt;
  (void)size;
  return (size_t)snprintf(buf, 100, "Year: %d, Month: %d, Day: %d, Hour: %d, Minute: %d, Second: %d\n", time_tm->tm_year + 1900, time_tm->tm_mon, time_tm->tm_mday, time_tm->tm_hour, time_tm->tm_min, time_tm->tm_sec);
};

/* VG pseudo */
/* Like touch to update last access timestamp (?) 
 * - deprecated. Actually defined in sys/time.h */
int utimes(const char *filename, const struct timeval timess[2]) {return 0;};
//#define utimes(X,Y) (void)X

/* VG pseudo */
//#define time(X) (*X)=CURRENT_TIME_SEC.tv_sec
time_t time (time_t* timer){return (time_t)0;};

