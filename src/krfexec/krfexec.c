#include <stdio.h>
#include <string.h>
#include <sys/resource.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/fcntl.h>
#include <errno.h>
#include <err.h>

#include "krfexec.h"

int main(int argc, char *argv[]) {
  if (argc < 2 || !strcmp(argv[1], "-h")) {
    printf("usage: krfexec <command or file> [args]\n");
    return 1;
  }

  krfexec_prep();

  struct rlimit core_limit;
  core_limit.rlim_cur = core_limit.rlim_max = RLIM_INFINITY;
  if (setrlimit(RLIMIT_CORE, &core_limit) < 0) {
    err(errno, "setrlimit");
  }

  if (execvp(argv[1], argv + 1) < 0) {
    err(errno, "exec %s", argv[1]);
  }

  return 0; /* noreturn */
}
