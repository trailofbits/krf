#include "internal.h"

#define KRF_SYS_CALL link
#define KRF_SYS_PARMS const char __user *oldname, const char __user *newname
#define KRF_SYS_PARMSX oldname, newname

DEFINE_FAULT(EACCES) {
  return -EACCES;
}

DEFINE_FAULT(EDQUOT) {
  return -EDQUOT;
}

DEFINE_FAULT(EFAULT) {
  return -EFAULT;
}

DEFINE_FAULT(EEXIST) {
  return -EEXIST;
}

DEFINE_FAULT(EIO) {
  return -EIO;
}

DEFINE_FAULT(ELOOP) {
  return -ELOOP;
}

DEFINE_FAULT(EMLINK) {
  return -EMLINK;
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

DEFINE_FAULT(ENOSPC) {
  return -ENOSPC;
}

DEFINE_FAULT(ENOTDIR) {
  return -ENOTDIR;
}

DEFINE_FAULT(EPERM) {
  return -EPERM;
}

DEFINE_FAULT(EROFS) {
  return -EROFS;
}

DEFINE_FAULT(EXDEV) {
  return -EXDEV;
}

static typeof(sys_link)(*fault_table[]) = {
    FAULT(EACCES), FAULT(EDQUOT),  FAULT(EFAULT),       FAULT(EEXIST), FAULT(EIO),
    FAULT(ELOOP),  FAULT(EMLINK),  FAULT(ENAMETOOLONG), FAULT(ENOENT), FAULT(ENOMEM),
    FAULT(ENOSPC), FAULT(ENOTDIR), FAULT(EPERM),        FAULT(EROFS),  FAULT(EXDEV),
};

// Fault entrypoint.
long KRF_DEFINE_INTERNAL(link)(KRF_SYS_PARMS) {
  return fault_table[KRF_LCG_NEXT() % NFAULTS](KRF_SYS_PARMSX);
}
