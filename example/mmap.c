#include "common.h"

int main(int argc, char const *argv[]) {
  unsigned int i;
  void *addr;

  for (i = 0;; i++) {
    if (i % 1000 == 0) {
      printf("iteration %u...\n", i);
    }

    if ((addr = mmap(NULL, 1024, PROT_READ, MAP_SHARED | MAP_ANONYMOUS, -1, 0)) == MAP_FAILED) {
      perror("fault!");
      exit(errno);
    }

    munmap(addr, 1024);
  }
  return 0;
}
