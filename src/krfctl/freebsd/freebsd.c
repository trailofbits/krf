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
#include <sys/sysctl.h>

#include "../krfctl.h"
#include "../../common/common.h"

/* control will interpret any number larger than its syscall table
 * as a command to clear all current masks.
 * it's a good bet that FreeBSD will never have 65535 syscalls.
 */
#define CLEAR_MAGIC 65535

#define CONTROL_NAME KRF_PROC_DIR "." KRF_CONTROL_FILENAME
#define RNG_STATE_NAME KRF_PROC_DIR "." KRF_RNG_STATE_FILENAME
#define PROBABILITY_NAME KRF_PROC_DIR "." KRF_PROBABILITY_FILENAME
#define LOG_FAULTS_NAME KRF_PROC_DIR "." KRF_LOG_FAULTS_FILENAME
#define TARGETING_NAME KRF_PROC_DIR "." KRF_TARGETING_FILENAME

int fault_syscall(const char *sys_name) {
  const char *sys_num;
  unsigned int syscall;

  if (!(sys_num = lookup_syscall_number(sys_name))) {
    warnx("WARNING: couldn't find syscall %s", sys_name);
    return 1;
  }

  if (sscanf(sys_num, "%u", &syscall) != 1) {
    err(errno, "weird syscall number");
  }

  if (sysctlbyname(CONTROL_NAME, NULL, NULL, &syscall, sizeof(syscall)) < 0) {
    if (errno == EOPNOTSUPP) {
      errx(errno, "faulting for %s unimplemented", sys_name);
    } else {
      err(errno, "sysctl " CONTROL_NAME);
    }
  }
  return 0;
}

void clear_faulty_calls(void) {
  unsigned int clr = CLEAR_MAGIC;
  if (sysctlbyname(CONTROL_NAME, NULL, NULL, &clr, sizeof(clr)) < 0) {
    err(errno, "write " CONTROL_NAME);
  }
}

void set_rng_state(const char *state) {
  unsigned int rng_state;

  if (sscanf(state, "%u", &rng_state) != 1) {
    err(1, "Weird rng_state");
  }

  if (sysctlbyname(RNG_STATE_NAME, NULL, NULL, &rng_state, sizeof(rng_state)) < 0) {
    err(errno, "write " RNG_STATE_NAME);
  }
}

void set_prob_state(const char *state) {
  unsigned int prob_state;

  if (sscanf(state, "%u", &prob_state) != 1) {
    err(1, "Weird prob_state");
  }

  if (sysctlbyname(PROBABILITY_NAME, NULL, NULL, &prob_state, sizeof(prob_state)) < 0) {
    err(errno, "write " PROBABILITY_NAME);
  }
}

void toggle_fault_logging(void) {
  unsigned int state;
  size_t amt_read = sizeof(state);
  if (sysctlbyname(LOG_FAULTS_NAME, &state, &amt_read, NULL, 0) < 0) {
    err(errno, "read " LOG_FAULTS_NAME);
  }

  state = !state;

  if (sysctlbyname(LOG_FAULTS_NAME, NULL, NULL, &state, sizeof(state)) < 0) {
    err(errno, "write " LOG_FAULTS_NAME);
  }
}

void set_targeting(unsigned int mode, const char *data) {
  char buf[32] = {0};
  if (snprintf(buf, sizeof(buf), "%u %s", mode, data) < 0) {
    err(errno, "snprintf");
  }

  if (sysctlbyname(TARGETING_NAME, NULL, NULL, &buf, strlen(buf)) < 0) {
    errx(errno, "write " TARGETING_NAME " - %s", buf);
  }
}
