#include <string.h>

char *strerror(int errnum){return NULL;};

/* Not safe. */
size_t strlcat(char * restr_dst, const char * restr_src, size_t size) {
  strcat(restr_dst, restr_src);
  return (size_t)strlen(restr_dst);
};

/* Not safe. */
size_t strlcpy(char * restr_dst, const char * restr_src, size_t size) {
  strcpy(restr_dst, restr_src);
  return (size_t)strlen(restr_dst);
};
