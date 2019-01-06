#include "common.h"

int main(int argc, char const *argv[]) {
  unsigned int i;
  char buf[4096];

  for (i = 0;; i++) {
    if (i % 1000 == 0) {
      printf("iteration %u...\n", i);
    }

    if (getcwd(buf, sizeof(buf)) == NULL) {
      perror("fault!");
      exit(errno);
    }
  }

  return 0;
}
