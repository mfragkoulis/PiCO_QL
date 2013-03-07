#include <time.h>
#include <linux/time.h>
#include <linux/kernel.h>

int gettimeofday(struct timeval *t, void *tzp) {
  do_gettimeofday(t);
  return 0;
};

struct tm * gmtime(const time_t * t) {
  static struct tm m;
  time_to_tm(*t, -2, &m);
  return &m;
};

struct tm * localtime(const time_t * t) {
  static struct tm m;
  time_to_tm(*t, 0, &m);
  return &m;
};

// Actually defined in sys/time.h
size_t strftime(char *buf, size_t size, const char *fmt, const struct tm *time_tm) {
  (void)fmt;
  (void)size;
  return (size_t)snprintf(buf, 100, "Year: %ld, Month: %d, Day: %d, Hour: %d, Minute: %d, Second: %d\n", time_tm->tm_year + 1900, time_tm->tm_mon, time_tm->tm_mday, time_tm->tm_hour, time_tm->tm_min, time_tm->tm_sec);
};
