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

#include "krfctl.h"
#include "../common/common.h"

const char *lookup_syscall_number(const char *sys_name) {
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

char *const targeting_opts[] = {[KRF_T_MODE_PERSONALITY] = "personality",
                                [KRF_T_MODE_PID] = "PID",
                                [KRF_T_MODE_UID] = "UID",
                                [KRF_T_MODE_GID] = "GID",
                                [KRF_T_MODE_INODE] = "INODE",
                                [KRF_T_NUM_MODES] = NULL};

int main(int argc, char *argv[]) {
  char *subopts, *value;
  int c;
  while ((c = getopt(argc, argv, "F:P:cr:p:LT:Ch")) != -1) {
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
        if (ca >= KRF_T_NUM_MODES) {
          printf("error: unknown targeting option %s\n", value);
          return 3;
        }
        set_targeting(ca, value);
      }
      break;
    }
    case 'C': {
      set_targeting(0, "0");
      break;
    }
    case 'h':
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
             " personality, PID, UID, GID, and INODE\n"
             "available profiles (for -P flag):\n"
             " ");
      fault_profile_t *elem = fault_profile_table;
      while (elem->profile != NULL) {
        printf("\t%s\t%s\n", elem->profile, elem->description);
        elem++;
      }
      return 1;
    }
    }
  }

  return 0;
}
