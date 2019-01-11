#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <err.h>

#include "krfctl.h"

#define CONTROL_FILE "/proc/krf/control"

/* control will interpret any number larger than its syscall table
 * as a command to clear all current masks.
 * it's a good bet that linux will never have 65535 syscalls.
 */
#define CLEAR_MAGIC "65535"

static const char *lookup_syscall_number(const char *sys_name) {
  for (syscall_lookup_t *elem = syscall_lookup_table; elem->sys_name != NULL; elem++) {
    if (!strcmp(sys_name, elem->sys_name)) {
      return elem->sys_num;
    }
  }

  return NULL;
}

int main(int argc, char const *argv[]) {
  int fd;
  const char *sys_name = NULL, *sys_num = NULL;

  if (argc != 2 || !strcmp(argv[1], "-h")) {
    printf("usage: krfctl <-c|spec>\n");
    return 1;
  }

  if ((fd = open(CONTROL_FILE, O_WRONLY)) < 0) {
    err(errno, "read " CONTROL_FILE);
    return errno;
  }

  if (!strcmp(argv[1], "-c")) {
    if (write(fd, CLEAR_MAGIC, strlen(CLEAR_MAGIC)) < 0) {
      err(errno, "write " CONTROL_FILE);
    }
  } else {
    char *spec = strdup(argv[1]);

    sys_name = strtok(spec, ", ");
    while (sys_name) {
      if (!(sys_num = lookup_syscall_number(sys_name))) {
        errx(1, "couldn't find syscall: %s", sys_name);
      }

      if (write(fd, sys_num, strlen(sys_num)) < 0) {
        /* friendly error message on unsupported syscall */
        if (errno == EOPNOTSUPP) {
          errx(errno, "faulting for %s unimplemented", sys_name);
        } else {
          err(errno, "write " CONTROL_FILE);
        }
      }

      sys_name = strtok(NULL, ", ");
    }

    free(spec);
  }

  close(fd);

  return 0;
}
