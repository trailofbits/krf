#pragma once

#include <errno.h>
#include <err.h>

/* TODO(ww): Put this in a common include directory.
 */
#define KRF_PERSONALITY 28

void krfexec_prep(void);
