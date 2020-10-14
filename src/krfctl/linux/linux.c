#include <stdio.h>
#include <string.h>
#include <sys/errno.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <err.h>
#include <string.h>

#include "../krfctl.h"
#include "../../common/common.h"

/* control will interpret any number larger than its syscall table
 * as a command to clear all current masks.
 * it's a good bet that linux will never have 65535 syscalls.
 */
#define CLEAR_MAGIC "65535"

#define CONTROL_FILE "/proc/" KRF_PROC_DIR "/" KRF_CONTROL_FILENAME
#define RNG_STATE_FILE "/proc/" KRF_PROC_DIR "/" KRF_RNG_STATE_FILENAME
#define PROBABILITY_FILE "/proc/" KRF_PROC_DIR "/" KRF_PROBABILITY_FILENAME
#define LOG_FAULTS_FILE "/proc/" KRF_PROC_DIR "/" KRF_LOG_FAULTS_FILENAME
#define TARGETING_FILE "/proc/" KRF_PROC_DIR "/" KRF_TARGETING_FILENAME

int fault_syscall(const char *sys_name) {
  int fd;
  const char *sys_num;

  /* check for wait4 and select */
  if (!strcmp(sys_name, "wait4") || !strcmp(sys_name, "select"))
    fprintf(stderr,
            "Warning: faulting syscall %s can potentially cause kernel oops on module unload\n",
            sys_name);

  /* TODO(ww): Opening the control file once per syscall is
   * pretty nasty, but I don't like passing a fd around.
   * Maybe a static variable that we test-and-set?
   */
  if ((fd = open(CONTROL_FILE, O_WRONLY)) < 0) {
    err(errno, "open " CONTROL_FILE);
  }

  if (!(sys_num = lookup_syscall_number(sys_name))) {
    warnx("WARNING: couldn't find syscall: %s", sys_name);
    return 1;
  }

  if (write(fd, sys_num, strlen(sys_num)) < 0) {
    /* friendly error message on unsupported syscall */
    if (errno == EOPNOTSUPP) {
      errx(errno, "faulting for %s unimplemented", sys_name);
    } else {
      err(errno, "write " CONTROL_FILE);
    }
  }

  close(fd);
  return 0;
}

void clear_faulty_calls(void) {
  int fd;

  if ((fd = open(CONTROL_FILE, O_WRONLY)) < 0) {
    err(errno, "open " CONTROL_FILE);
  }

  if (write(fd, CLEAR_MAGIC, strlen(CLEAR_MAGIC)) < 0) {
    err(errno, "write " CONTROL_FILE);
  }

  close(fd);
}

void set_rng_state(const char *state) {
  int fd;

  if ((fd = open(RNG_STATE_FILE, O_WRONLY)) < 0) {
    err(errno, "open " RNG_STATE_FILE);
  }

  if (write(fd, state, strlen(state)) < 0) {
    err(errno, "write " CONTROL_FILE);
  }

  close(fd);
}

void set_prob_state(const char *state) {
  int fd;

  if ((fd = open(PROBABILITY_FILE, O_WRONLY)) < 0) {
    err(errno, "open " PROBABILITY_FILE);
  }

  if (write(fd, state, strlen(state)) < 0) {
    err(errno, "write " CONTROL_FILE);
  }

  close(fd);
}

void toggle_fault_logging(void) {
  int fd;
  char buf[32] = {0};
  unsigned int state;

  if ((fd = open(LOG_FAULTS_FILE, O_RDWR)) < 0) {
    err(errno, "open " LOG_FAULTS_FILE);
  }

  if (read(fd, buf, sizeof(buf) - 1) < 0) {
    err(errno, "read " LOG_FAULTS_FILE);
  }

  if (sscanf(buf, "%u", &state) != 1) {
    errx(1, "weird logging state: %s", buf);
  }

  state = !state;
  memset(buf, 0, sizeof(buf));
  snprintf(buf, sizeof(buf), "%u", state);

  if (write(fd, buf, strlen(buf)) < 0) {
    err(errno, "write " LOG_FAULTS_FILE);
  }

  close(fd);
}

void set_targeting(unsigned int mode, const char *data) {
  int fd;
  char buf[32] = {0};
  if ((fd = open(TARGETING_FILE, O_WRONLY)) < 0) {
    err(errno, "open " TARGETING_FILE);
  }

  if (snprintf(buf, sizeof(buf), "%u %s", mode, data) < 0) {
    err(errno, "snprintf");
  }

  if (write(fd, buf, strlen(buf)) < 0) {
    err(errno, "write " TARGETING_FILE);
  }

  close(fd);
}
