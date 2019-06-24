#include <unistd.h>
#include <sys/types.h>
#include <sys/sysctl.h>
#include <stdio.h>
#include <errno.h>
#include <err.h>
#include <string.h>

#include "../krfexec.h"

void krfexec_prep(void) {
  char buf[32] = {0};
  pid_t pid;

  pid = getpid();

  if (snprintf(buf, 32, "1 %u", (unsigned int)pid) < 0) {
    err(errno, "snprintf");
  }

  if (sysctlbyname(KRF_TARGETING_NAME, NULL, NULL, &buf, strnlen(buf, 32)) < 0) {
    err(errno, "sysctl failed");
  }

  /* TODO(hw)
   * This must be run as root to successfully do sysctl, so setting euid and egid
   * back to the real uid and real gid will allow programs to be executed without
   * potentially dangerous root privileges.
   */
}
