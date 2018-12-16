#include "internal.h"

#define KRF_SYS_CALL clone
#define KRF_SYS_PARMS                                                                              \
  unsigned long clone_flags, unsigned long newsp, int __user *parent_tidptr,                       \
      int __user *child_tidptr, unsigned long tls
#define KRF_SYS_PARMSX clone_flags, newsp, parent_tidptr, child_tidptr, tls

DEFINE_FAULT(EAGAIN) {
  return -EAGAIN;
}

DEFINE_FAULT(EINVAL) {
  return -EINVAL;
}

DEFINE_FAULT(ENOMEM) {
  return -ENOMEM;
}

DEFINE_FAULT(ENOSPC) {
  return -ENOSPC;
}

/* Linux >= 3.11, <= 4.8
 */
DEFINE_FAULT(EUSERS) {
  return -EUSERS;
}

DEFINE_FAULT(EPERM) {
  return -EPERM;
}

DEFINE_FAULT(ERESTARTNOINTR) {
  return -ERESTARTNOINTR;
}

static typeof(sys_clone)(*fault_table[]) = {
    FAULT(EAGAIN), FAULT(EINVAL), FAULT(ENOMEM),         FAULT(ENOSPC),
    FAULT(EUSERS), FAULT(EPERM),  FAULT(ERESTARTNOINTR),
};

// Fault entrypoint.
long KRF_DEFINE_INTERNAL(clone)(KRF_SYS_PARMS) {
  return fault_table[KRF_LCG_NEXT() % NFAULTS](KRF_SYS_PARMSX);
}
