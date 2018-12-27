#include "common.h"

int main(int argc, char const *argv[]) {
  unsigned int i;
  int fd;
  const char *file = "/tmp/krf_creat";

  for (i = 0;; i++) {
    if (i % 1000 == 0) {
      printf("iteration %u...\n", i);
    }

    if ((fd = creat(file, 0700)) < 0) {
      perror("fault!");
      exit(errno);
    }

    close(fd);
    unlink(file);
  }

  return 0;
}
