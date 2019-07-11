#pragma once
#include "config.h"
#ifdef LINUX
#include "linux/linux.h"
#endif
#ifdef __FreeBSD__
#include "freebsd/freebsd.h"
#endif

static __always_inline int krf_targeted(KRF_TARGETING_PROTO) {
  int targeted = 1;
  size_t i = 0;
  for (; i < KRF_T_NUM_MODES; i++) {
    if (targeted == 0)
      break;

    if (krf_target_options.mode_mask & (1 << i)) {
      switch (i) {
      case KRF_T_MODE_PERSONALITY:
        if (KRF_PERSONALITY(krf_target_options.target_data[i]))
          targeted++;
        else
          targeted = 0;
        break;
      case KRF_T_MODE_PID:
        if (KRF_PID(krf_target_options.target_data[i]))
          targeted++;
        else
          targeted = 0;
        break;
      case KRF_T_MODE_UID:
        if (KRF_UID(krf_target_options.target_data[i]))
          targeted++;
        else
          targeted = 0;
        break;
      case KRF_T_MODE_GID:
        if (KRF_GID(krf_target_options.target_data[i]))
          targeted++;
        else
          targeted = 0;
        break;
      }
    }
  }
  return (targeted & (~1));
}
