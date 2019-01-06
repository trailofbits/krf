#include "common.h"

int main(int argc, char const *argv[]) {
  unsigned int i;
  struct stat statbuf;

  for (i = 0;; i++) {
    if (i % 1000 == 0) {
      printf("iteration %u...\n", i);
    }

    if (stat("/tmp", &statbuf) < 0) {
      perror("fault!");
      exit(errno);
    }
  }

  return 0;
}
