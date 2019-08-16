#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/sysctl.h>
#include <stdio.h>
#include <errno.h>
#include <err.h>
#include <string.h>
#include <pwd.h>

#include "../krfexec.h"
#include "../../common/common.h"

void krfexec_prep(void) {
  char buf[32] = {0};
  pid_t pid = getpid();
  if (snprintf(buf, 32, "1 %u", (unsigned int)pid) < 0) {
    errx(1, "snprintf");
  }

  if (sysctlbyname(KRF_PROC_DIR "." KRF_TARGETING_FILENAME, NULL, NULL, &buf, strnlen(buf, 32)) <
      0) {
    err(errno, "sysctl failed");
  }
}
