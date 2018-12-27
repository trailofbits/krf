#include "internal.h"

#define KRF_SYS_CALL mkdir
#define KRF_SYS_PARMS const char __user *pathname, umode_t mode
#define KRF_SYS_PARMSX pathname, mode

DEFINE_FAULT(EACCES) {
  return -EACCES;
}

DEFINE_FAULT(EFAULT) {
  return -EFAULT;
}

DEFINE_FAULT(EDQUOT) {
  return -EDQUOT;
}

DEFINE_FAULT(ELOOP) {
  return -ELOOP;
}

DEFINE_FAULT(ENAMETOOLONG) {
  return -ENAMETOOLONG;
}

DEFINE_FAULT(ENOENT) {
  return -ENOENT;
}

DEFINE_FAULT(ENOMEM) {
  return -ENOMEM;
}

DEFINE_FAULT(ENOTDIR) {
  return -ENOTDIR;
}

DEFINE_FAULT(EBADF) {
  return -EBADF;
}

DEFINE_FAULT(EEXIST) {
  return -EEXIST;
}

DEFINE_FAULT(EMLINK) {
  return -EMLINK;
}

DEFINE_FAULT(ENOSPC) {
  return -ENOSPC;
}

DEFINE_FAULT(EPERM) {
  return -EPERM;
}

DEFINE_FAULT(EROFS) {
  return -EROFS;
}

static typeof(sys_mkdir)(*fault_table[]) = {
    FAULT(EACCES), FAULT(EFAULT), FAULT(EDQUOT),  FAULT(ELOOP), FAULT(ENAMETOOLONG),
    FAULT(ENOENT), FAULT(ENOMEM), FAULT(ENOTDIR), FAULT(EBADF), FAULT(EEXIST),
    FAULT(EMLINK), FAULT(ENOSPC), FAULT(EPERM),   FAULT(EROFS),
};

// Fault entrypoint.
long KRF_DEFINE_INTERNAL(mkdir)(KRF_SYS_PARMS) {
  return fault_table[KRF_LCG_NEXT() % NFAULTS](KRF_SYS_PARMSX);
}
