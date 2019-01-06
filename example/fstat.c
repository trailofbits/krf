#include "common.h"

int main(int argc, char const *argv[]) {
  unsigned int i;
  int fd;
  struct stat statbuf;

  fd = open("/etc/hostname", O_RDONLY);

  for (i = 0;; i++) {
    if (i % 1000 == 0) {
      printf("iteration %u...\n", i);
    }

    if (fstat(fd, &statbuf) < 0) {
      perror("fault!");
      exit(errno);
    }
  }

  return 0;
}
