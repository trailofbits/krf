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
#define RNG_STATE_FILE "/proc/krf/rng_state"
#define PROBABILITY_FILE "/proc/krf/probability"

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

static void fault_syscall_spec(const char *s) {
  int fd;
  const char *sys_name = NULL, *sys_num = NULL;

  if ((fd = open(CONTROL_FILE, O_WRONLY)) < 0) {
    err(errno, "open " CONTROL_FILE);
  }

  char *spec = strdup(s);

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
  close(fd);
}

static void clear_faulty_calls(void) {
  int fd;

  if ((fd = open(CONTROL_FILE, O_WRONLY)) < 0) {
    err(errno, "open " CONTROL_FILE);
  }

  if (write(fd, CLEAR_MAGIC, strlen(CLEAR_MAGIC)) < 0) {
    err(errno, "write " CONTROL_FILE);
  }

  close(fd);
}

static void set_rng_state(const char *state) {
  int fd;

  if ((fd = open(RNG_STATE_FILE, O_WRONLY)) < 0) {
    err(errno, "open " RNG_STATE_FILE);
  }

  if (write(fd, state, strlen(state)) < 0) {
    err(errno, "write " CONTROL_FILE);
  }

  close(fd);
}

static void set_prob_state(const char *state) {
  int fd;

  if ((fd = open(PROBABILITY_FILE, O_WRONLY)) < 0) {
    err(errno, "open " PROBABILITY_FILE);
  }

  if (write(fd, state, strlen(state)) < 0) {
    err(errno, "write " CONTROL_FILE);
  }

  close(fd);
}

int main(int argc, char *argv[]) {

  int c;
  while ((c = getopt(argc, argv, "F:cr:p:")) != -1) {
    switch (c) {
    case 'F': {
      fault_syscall_spec(optarg);
      break;
    }
    case 'c': {
      clear_faulty_calls();
      break;
    }
    case 'r': {
      set_rng_state(optarg);
      break;
    }
    case 'p': {
      set_prob_state(optarg);
      break;
    }
    default: {
      printf("usage: krfctl <options>\n"
             "options:\n"
             " -h                          display this help message\n"
             " -F <syscall> [syscall...]   fault the given syscalls\n"
             " -c                          clear the syscall table of faulty calls\n"
             " -r <state>                  set the RNG state\n"
             " -p <prob>                   set the fault probability\n");
      return 1;
    }
    }
  }

  return 0;
}
