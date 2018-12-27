#include "internal.h"

#define KRF_SYS_CALL rmdir
#define KRF_SYS_PARMS const char __user *pathname
#define KRF_SYS_PARMSX pathname

DEFINE_FAULT(EACCES) {
  return -EACCES;
}

DEFINE_FAULT(EFAULT) {
  return -EFAULT;
}

DEFINE_FAULT(EBUSY) {
  return -EBUSY;
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

DEFINE_FAULT(EINVAL) {
  return -EINVAL;
}

DEFINE_FAULT(ENOTEMPTY) {
  return -ENOTEMPTY;
}

DEFINE_FAULT(EPERM) {
  return -EPERM;
}

DEFINE_FAULT(EROFS) {
  return -EROFS;
}

static typeof(sys_rmdir)(*fault_table[]) = {
    FAULT(EACCES),       FAULT(EFAULT),    FAULT(EBUSY),  FAULT(ELOOP),
    FAULT(ENAMETOOLONG), FAULT(ENOENT),    FAULT(ENOMEM), FAULT(ENOTDIR),
    FAULT(EINVAL),       FAULT(ENOTEMPTY), FAULT(EPERM),  FAULT(EROFS),
};

// Fault entrypoint.
long KRF_DEFINE_INTERNAL(rmdir)(KRF_SYS_PARMS) {
  return fault_table[KRF_LCG_NEXT() % NFAULTS](KRF_SYS_PARMSX);
}
