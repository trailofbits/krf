#include "common.h"

int main(int argc, char const *argv[]) {
  unsigned int i;
  int fd;

  for (i = 0;; i++) {
    if (i % 1000 == 0) {
      printf("iteration %u...\n", i);
    }

    /* As noted in krf.c, we can't use the glibc wrapper here --
       it maps to openat(2) instead.
     */
    if ((fd = syscall(SYS_open, "/dev/zero", O_RDONLY)) < 0) {
      perror("fault!");
      exit(errno);
    }

    close(fd);
  }

  return 0;
}
