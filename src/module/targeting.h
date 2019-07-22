#pragma once
#include "config.h"
#ifdef LINUX
#include "linux/linux.h"
#include "linux/targeting.h"
#endif
#ifdef __FreeBSD__
#include "freebsd/freebsd.h"
#include "freebsd/targeting.h"
#endif

static __always_inline int krf_targeted(krf_ctx_t *context) {
  int targeted = 1;
  size_t i = 0;
  for (; i < KRF_T_NUM_MODES; i++) {
    if (targeted == 0)
      break;

    if (krf_target_options.mode_mask & (1 << i)) {
      switch (i) {
      case KRF_T_MODE_PERSONALITY:
        if (krf_personality(krf_target_options.target_data[i], context))
          targeted++;
        else
          targeted = 0;
        break;
      case KRF_T_MODE_PID:
        if (krf_pid(krf_target_options.target_data[i], context))
          targeted++;
        else
          targeted = 0;
        break;
      case KRF_T_MODE_UID:
        if (krf_uid(krf_target_options.target_data[i], context))
          targeted++;
        else
          targeted = 0;
        break;
      case KRF_T_MODE_GID:
        if (krf_gid(krf_target_options.target_data[i], context))
          targeted++;
        else
          targeted = 0;
        break;
      case KRF_T_MODE_INODE:
        if (krf_inode(krf_target_options.target_data[i], context))
          targeted++;
        else
          targeted = 0;
        break;
      }
    }
  }
  return (targeted & (~1));
}
