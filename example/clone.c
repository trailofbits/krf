#include "common.h"

int main(int argc, char const *argv[]) {
  unsigned int i;
  pid_t pid;
  int status;

  /* I'm too lazy to figure out the exact clone(3) arguments, so just use fork(3)
   * since it's a clone(2) wrapper internally on glibc.
   */
  for (i = 0;; i++) {
    if (i % 1000 == 0) {
      printf("iteration %u...\n", i);
    }

    switch ((pid = fork())) {
    case -1: {
      perror("fault!");
      exit(errno);
      break;
    }
    case 0: {
      /* Child */
      exit(0);
      break;
    }
    default: {
      wait(&status);
      break;
    }
    }
  }
  return 0;
}
