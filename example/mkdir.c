#include "common.h"

int main(int argc, char const *argv[]) {
  unsigned int i;

  const char *dir = "/tmp/foo";

  for (i = 0;; i++) {
    if (i % 1000 == 0) {
      printf("iteration %u...\n", i);
    }

    if (mkdir(dir, 0700) < 0) {
      perror("fault!");
      exit(errno);
    }

    rmdir(dir);
  }

  return 0;
}
