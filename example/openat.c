#include "common.h"

int main(int argc, char const *argv[]) {
  unsigned int i;
  int fd;

  for (i = 0;; i++) {
    if (i % 1000 == 0) {
      printf("iteration %u...\n", i);
    }

    if ((fd = openat(AT_FDCWD, "/dev/zero", O_RDONLY)) < 0) {
      perror("fault!");
      exit(errno);
    }

    close(fd);
  }

  return 0;
}
