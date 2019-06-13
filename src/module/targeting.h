#pragma once
#include "config.h"
#ifdef LINUX
#include "linux/linux.h"
#endif

static __always_inline int krf_targeted(void) {
  int targeted = 1;
  size_t i = 0;
  for (; i < KRF_T_NUM_MODES; i++) {
    if (targeted == 0)
      break;

    if (krf_target_options.mode_mask & (1 << i)) {
      switch (i) {
      case KRF_T_MODE_PERSONALITY:
        if ((PERSONALITY() & krf_target_options.target_data[i]) != 0)
          targeted++;
        else
          targeted = 0;
        break;
      case KRF_T_MODE_PID:
        if (PID() == krf_target_options.target_data[i])
          targeted++;
        else
          targeted = 0;
        break;
      case KRF_T_MODE_UID:
        if (UID() == krf_target_options.target_data[i])
          targeted++;
        else
          targeted = 0;
        break;
      case KRF_T_MODE_GID:
        if (GID() == krf_target_options.target_data[i])
          targeted++;
        else
          targeted = 0;
        break;
      }
    }
  }
  return (targeted & (~1));
}
