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

/* This employs a collection of hacks to attempt to drop down
 * to the "originating" user, including:
 *
 * 1. Checking for SUDO_UID, and using its value
 * 2. Checking for REAL_UID, which we document
 * 3. Checking getlogin_r, which returns the username for the controlling terminal
 */
static uid_t guess_original_uid(void) {
  uid_t uid;

  const char *uid_s = getenv("SUDO_UID");
  if (uid_s != NULL) {
    if (sscanf(uid_s, "%u", &uid) != 1) {
      errx(1, "weird SUDO_UID value: %s", uid_s);
    }
    return uid;
  }

  uid_s = getenv("REAL_UID");
  if (uid_s != NULL) {
    if (sscanf(uid_s, "%u", &uid) != 1) {
      errx(1, "weird REAL_UID value: %s", uid_s);
    }
    return uid;
  }

  char login[L_cuserid] = {0};
  if (getlogin_r(login, L_cuserid) == 0) {
    const struct passwd *pwd = getpwnam(login);
    if (pwd != NULL) {
      return pwd->pw_uid;
    }
  }

  errx(1, "barf: all guessing methods failed, set REAL_UID manually");
}

void krfexec_prep(void) {
  if (getuid() != 0) {
    errx(1, "krfexec should be run as root on freebsd");
  }

  char buf[32] = {0};
  pid_t pid = getpid();
  if (snprintf(buf, 32, "1 %u", (unsigned int)pid) < 0) {
    errx(1, "snprintf");
  }

  if (sysctlbyname(KRF_TARGETING_NAME, NULL, NULL, &buf, strnlen(buf, 32)) < 0) {
    err(errno, "sysctl failed");
  }

  uid_t uid = guess_original_uid();
  if (uid != getuid()) {
    if (setuid(uid) < 0) {
      err(errno, "setuid %u", uid);
    }
  }
}
