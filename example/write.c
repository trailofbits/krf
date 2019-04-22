#include "common.h"

int main(int argc, char const *argv[]) {
  unsigned int i;
  int fd;
  char buf = 0x41;

  fd = open("/dev/null", O_WRONLY);

  for (i = 0;; i++) {
    if (i % 1000 == 0) {
      printf("iteration %u...\n", i);
    }

    if (write(fd, &buf, 1) < 0) {
      perror("fault!");
      exit(errno);
    }
  }
  return 0;
}
