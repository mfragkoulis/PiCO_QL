#include <stdlib.h>

// Get environment variable (needed?)
char * getenv(const char *str) {
  static char tmpdir[] = "/tmp";
  if (!strcmp(str, "TMPDIR"))
    return tmpdir;
  else
    return NULL;
};
