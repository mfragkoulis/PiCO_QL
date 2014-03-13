#include <stdlib.h>
#include <linux/slab.h>

unsigned long memMaxFootprint = 0;
unsigned long memFootprint = 0;

void calc_max(void) {
  if (memMaxFootprint < memFootprint)
    memMaxFootprint = memFootprint;
}

void * instr_malloc(size_t size) {
  memFootprint += size;
  calc_max();
  return kmalloc(size, GFP_KERNEL);
};

void * instr_realloc(void *ptr, size_t size) {
  memFootprint += (size - sizeof(ptr));
  calc_max();
  return krealloc(ptr, size, GFP_KERNEL);
};

void instr_free(void *ptr) {
  memFootprint -= sizeof(ptr);
  kfree(ptr);
};

// Get environment variable (needed?)
char * getenv(const char *str) {
  static char tmpdir[] = "/tmp";
  if (!strcmp(str, "TMPDIR"))
    return tmpdir;
  else
    return NULL;
};
