#include "common.h"

int main(int argc, char const *argv[]) {
  unsigned int i;
  int fd;
  char buf;

  fd = open("/dev/zero", O_RDONLY);

  for (i = 0;; i++) {
    if (i % 1000 == 0) {
      printf("iteration %u...\n", i);
    }

    if (read(fd, &buf, 1) < 0) {
      perror("fault!");
      exit(errno);
    }
  }
  return 0;
}
