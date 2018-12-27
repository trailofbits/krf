#include "internal.h"

#define KRF_SYS_CALL fchdir
#define KRF_SYS_PARMS unsigned int fd
#define KRF_SYS_PARMSX fd

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

DEFINE_FAULT(EBADF) {
  return -EBADF;
}

static typeof(sys_fchdir)(*fault_table[]) = {
    FAULT(EACCES), FAULT(EFAULT), FAULT(EIO),     FAULT(ELOOP), FAULT(ENAMETOOLONG),
    FAULT(ENOENT), FAULT(ENOMEM), FAULT(ENOTDIR), FAULT(EBADF),
};

// Fault entrypoint.
long KRF_DEFINE_INTERNAL(fchdir)(KRF_SYS_PARMS) {
  return fault_table[KRF_LCG_NEXT() % NFAULTS](KRF_SYS_PARMSX);
}
