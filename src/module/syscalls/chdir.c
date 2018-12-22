#include "internal.h"

#define KRF_SYS_CALL chdir
#define KRF_SYS_PARMS const char __user *filename
#define KRF_SYS_PARMSX filename

DEFINE_FAULT(EACCES) {
  return -EACCES;
}

DEFINE_FAULT(EFAULT) {
  return -EFAULT;
}

DEFINE_FAULT(EIO) {
  return -EIO;
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

static typeof(sys_chdir)(*fault_table[]) = {
    FAULT(EACCES),       FAULT(EFAULT), FAULT(EIO),    FAULT(ELOOP),
    FAULT(ENAMETOOLONG), FAULT(ENOENT), FAULT(ENOMEM), FAULT(ENOTDIR),
};

// Fault entrypoint.
long KRF_DEFINE_INTERNAL(chdir)(KRF_SYS_PARMS) {
  return fault_table[KRF_LCG_NEXT() % NFAULTS](KRF_SYS_PARMSX);
}
