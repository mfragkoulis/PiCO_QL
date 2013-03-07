/* Compile
 * gcc -g picoQL_config_ioctl.c -o picoQL_config_ioctl
 */

#include <sys/ioctl.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char **argv) {
  int picoQL_fd;
  int picoQL_ioctl_code;
  if (argc > 2) {
    printf("Too many arguments: expected 1, got %i.\n", argc - 1);
    exit(1);
  } else if (argc < 2) {
    printf("Argument missing: expected 1, got %i.\n", argc - 1);
    exit(1);
  }
  picoQL_ioctl_code = atoi(argv[1]);
  picoQL_fd = open("/proc/picoQL", 0);
  if (picoQL_fd < 0) {
    printf("Unable to open /proc/picoQL.\n");
    exit(1);
  }
  printf("ioctl returned %i for /proc/picoQL.\n", ioctl(picoQL_fd, picoQL_ioctl_code, 0));
  close(picoQL_fd);
}
