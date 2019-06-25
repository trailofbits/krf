#pragma once

#include <errno.h>
#include <err.h>

/* TODO(ww): Put this in a common include directory.
 */
#define KRF_PERSONALITY 28
#define KRF_TARGETING_NAME "krf.targeting"

void krfexec_prep(void);
