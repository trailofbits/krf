#include "common.h"

int main(int argc, char const *argv[]) {
  unsigned int i;
  int fd;

  for (i = 0;; i++) {
    if (i % 1000 == 0) {
      printf("iteration %u...\n", i);
    }

    fd = open("/dev/zero", O_RDONLY);

    if (close(fd) < 0) {
      perror("fault!");
      exit(errno);
    }
  }

  return 0;
}
