#include <sys/personality.h>

#include "../krfexec.h"

void krfexec_prep(void) {
  if (personality(KRF_PERSONALITY | ADDR_NO_RANDOMIZE) < 0) {
    err(errno, "personality");
  }

  /* TODO(ww): Maybe disable the VDSO?
   * Here's how we could do it on a per-process basis: https://stackoverflow.com/a/52402306
   */
}
