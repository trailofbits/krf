#include "common.h"

int main(int argc, char const *argv[]) {
  unsigned int i;
  const char *filename = "/tmp/krf_symlink1";
  const char *linkname = "/tmp/krf_symlink2";

  for (i = 0;; i++) {
    if (i % 1000 == 0) {
      printf("iteration %u...\n", i);
    }

    creat(filename, 0700);

    if (symlink(filename, linkname) < 0) {
      perror("fault!");
      unlink(filename);
      unlink(linkname);
      exit(errno);
    }

    unlink(filename);
    unlink(linkname);
  }

  return 0;
}
