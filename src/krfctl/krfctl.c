#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <err.h>
#include <string.h>

#include "krfctl.h"

#define CONTROL_FILE "/proc/krf/control"
#define RNG_STATE_FILE "/proc/krf/rng_state"
#define PROBABILITY_FILE "/proc/krf/probability"
#define LOG_FAULTS_FILE "/proc/krf/log_faults"
#define TARGETING_FILE "/proc/krf/targeting"

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

static const char **lookup_syscall_profile(const char *profile) {
  for (fault_profile_t *elem = fault_profile_table; elem->profile != NULL; elem++) {
    if (!strcmp(profile, elem->profile)) {
      return elem->syscalls;
    }
  }

  return NULL;
}

static void fault_syscall(const char *sys_name) {
  int fd;
  const char *sys_num;

  /* TODO(ww): Opening the control file once per syscall is
   * pretty nasty, but I don't like passing a fd around.
   * Maybe a static variable that we test-and-set?
   */
  if ((fd = open(CONTROL_FILE, O_WRONLY)) < 0) {
    err(errno, "open " CONTROL_FILE);
  }

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

  close(fd);
}

static void fault_syscall_spec(const char *s) {
  const char *sys_name = NULL;

  char *spec = strdup(s);

  sys_name = strtok(spec, ", ");
  while (sys_name) {
    fault_syscall(sys_name);
    sys_name = strtok(NULL, ", ");
  }

  free(spec);
}

static void fault_syscall_profile(const char *profile) {
  const char **syscalls = lookup_syscall_profile(profile);

  if (syscalls == NULL) {
    errx(1, "couldn't find fault profile: %s", profile);
  }

  int i;
  for (i = 0; syscalls[i]; i++) {
    fault_syscall(syscalls[i]);
  }
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

static void toggle_fault_logging(void) {
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

static void set_targeting(unsigned int mode, const char *data) {
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

enum
{
 TARGET_PERSONALITY = 0,
 TARGET_PID,
 TARGET_UID,
 TARGET_GID,
 TARGET_NUM_MODES
};

const char *targeting_opts[] =
{
 [TARGET_PERSONALITY] = "personality",
 [TARGET_PID] = "PID",
 [TARGET_UID] = "UID",
 [TARGET_GID] = "GID",
 [TARGET_NUM_MODES] = NULL
};

int main(int argc, char *argv[]) {
  char *subopts, *value;
  int c;
  while ((c = getopt(argc, argv, "F:P:cr:p:LT:C")) != -1) {
    switch (c) {
    case 'F': {
      fault_syscall_spec(optarg);
      break;
    }
    case 'P': {
      fault_syscall_profile(optarg);
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
    case 'L': {
      toggle_fault_logging();
      break;
    }
    case 'T': {
      subopts = optarg;
      int ca;
      while (*subopts != '\0') {
	ca = getsubopt(&subopts, targeting_opts, &value);
	if (value == NULL) {
	  printf("error: there must be a value input for the targeting option\n");
	  return 2;
	}
	if (ca >= TARGET_NUM_MODES) {
	  printf("error: unknown targeting option %s\n", value);
	  return 3;
	}
	set_targeting(ca, value);
      }
      break;
    }
    case 'C': {
      set_targeting(0,"0");
      break;
    }
    default: {
      printf("usage: krfctl <options>\n"
             "options:\n"
             " -h                          display this help message\n"
             " -F <syscall> [syscall...]   fault the given syscalls\n"
             " -P <profile>                fault the given syscall profile\n"
             " -c                          clear the syscall table of faulty calls\n"
             " -r <state>                  set the RNG state\n"
             " -p <prob>                   set the fault probability\n"
             " -L                          toggle faulty call logging\n"
	     " -T <variable>=<value>       enable targeting option <variable> with value <value>\n"
	     " -C                          clear the targeting options\n"
	     "targeting options:\n"
	     " personality, PID, UID, and GID\n"
	     );
      return 1;
    }
    }
  }

  return 0;
}
