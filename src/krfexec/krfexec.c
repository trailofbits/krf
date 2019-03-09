#include <stdio.h>
#include <string.h>
#include <sys/resource.h>
#include <sys/personality.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/fcntl.h>
#include <err.h>

#define PERSONALITY_FILE "/proc/krf/personality"

int main(int argc, char *argv[]) {
  if (argc < 2 || !strcmp(argv[1], "-h")) {
    printf("usage: krfexec <command or file> [args]\n");
    return 1;
  }

  int fd;
  if ((fd = open(PERSONALITY_FILE, O_RDONLY)) < 0) {
    err(errno, "open " PERSONALITY_FILE);
  }

  char pers_str[33] = {0};
  if (read(fd, pers_str, sizeof(pers_str) - 1) < 0) {
    err(errno, "read " PERSONALITY_FILE);
  }

  unsigned int pers;
  if (sscanf(pers_str, "%u", &pers) != 1) {
    errx(1, "oddity: personality isn't numeric?");
  }

  if (personality(pers | ADDR_NO_RANDOMIZE) < 0) {
    err(errno, "personality");
  }

  struct rlimit core_limit;
  core_limit.rlim_cur = core_limit.rlim_max = RLIM_INFINITY;
  if (setrlimit(RLIMIT_CORE, &core_limit) < 0) {
    err(errno, "setrlimit");
  }

  if (execvp(argv[1], argv + 1) < 0) {
    err(errno, "exec %s", argv[1]);
  }

  /* TODO(ww): Maybe disable the VDSO?
   * Here's how we could do it on a per-process basis: https://stackoverflow.com/a/52402306
   */

  return 0; /* noreturn */
}
