#include <sys/errno.h>

#ifdef __linux__

int *__error(void);
int *__errno_location(void);

int *__errno_location(void) {
  return (int *)0;
}
#endif


int *__error(void) {
  return (int *)0;
}

