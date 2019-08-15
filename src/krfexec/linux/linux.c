#include <sys/personality.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>

#include "../krfexec.h"
#include "../../common/common.h"

#define TARGETING_FILE "/proc/" KRF_PROC_DIR "/" KRF_TARGETING_FILENAME

void krfexec_prep(void) {
  // Check if personality is being targeted
  int fd;
  char buf[64] = {0};
  int set = 0;
  if ((fd = open(TARGETING_FILE, O_RDONLY)) < 0) {
    err(errno, "open " TARGETING_FILE);
  }

  if (read(fd, buf, sizeof(buf) - 1) < 0) {
    err(errno, "read" TARGETING_FILE);
  }

  unsigned mode, data;
  while (sscanf(buf, "%u %u", &mode, &data) == 2) {
    if (mode != KRF_T_MODE_PERSONALITY)
      continue;

    if (data == KRF_PERSONALITY) {
      set = 1;
      break;
    } else {
      errx(1, "Personality set to a value that krfexec does not recognize. Use `krfctl -T "
              "personality=28` to properly set.");
    }
  }

  if (!set) {
    errx(1, "Personality targeting disabled. Run `krfctl -T personality=28` to enable.");
  }

  close(fd);

  if (personality(KRF_PERSONALITY | ADDR_NO_RANDOMIZE) < 0) {
    err(errno, "personality");
  }

  /* TODO(ww): Maybe disable the VDSO?
   * Here's how we could do it on a per-process basis: https://stackoverflow.com/a/52402306
   */
}
