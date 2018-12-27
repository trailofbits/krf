#include "internal.h"

#define KRF_SYS_CALL creat
#define KRF_SYS_PARMS const char __user *pathname, umode_t mode
#define KRF_SYS_PARMSX pathname, mode

DEFINE_FAULT(EACCES) {
  return -EACCES;
}

DEFINE_FAULT(EDQUOT) {
  return -EDQUOT;
}

// EEXIST?

DEFINE_FAULT(EFAULT) {
  return -EFAULT;
}

DEFINE_FAULT(EFBIG) {
  return -EFBIG;
}

DEFINE_FAULT(EINTR) {
  return -EINTR;
}

DEFINE_FAULT(EINVAL) {
  return -EINVAL;
}

// EISDIR?

DEFINE_FAULT(ELOOP) {
  return -ELOOP;
}

DEFINE_FAULT(EMFILE) {
  return -EMFILE;
}

DEFINE_FAULT(ENAMETOOLONG) {
  return -ENAMETOOLONG;
}

DEFINE_FAULT(ENFILE) {
  return -ENFILE;
}

// ENODEV?
// ENOENT?

DEFINE_FAULT(ENOMEM) {
  return -ENOMEM;
}

DEFINE_FAULT(ENOSPC) {
  return -ENOSPC;
}

DEFINE_FAULT(ENOTDIR) {
  return -ENOTDIR;
}

// ENXIO?

DEFINE_FAULT(EOVERFLOW) {
  return -EOVERFLOW;
}

DEFINE_FAULT(EPERM) {
  return -EPERM;
}

DEFINE_FAULT(EROFS) {
  return -EROFS;
}

DEFINE_FAULT(ETXTBSY) {
  return -ETXTBSY;
}

// EWOULDBLOCK?

DEFINE_FAULT(EBADF) {
  return -EBADF;
}

// ENOTDIR?

static typeof(sys_creat)(*fault_table[]) = {
    FAULT(EACCES), FAULT(EDQUOT),  FAULT(EFAULT),  FAULT(EFBIG),        FAULT(EINTR),
    FAULT(EINVAL), FAULT(ELOOP),   FAULT(EMFILE),  FAULT(ENAMETOOLONG), FAULT(ENFILE),
    FAULT(ENOMEM), FAULT(ENOSPC),  FAULT(ENOTDIR), FAULT(EOVERFLOW),    FAULT(EPERM),
    FAULT(EROFS),  FAULT(ETXTBSY), FAULT(EBADF),
};

// Fault entrypoint.
long KRF_DEFINE_INTERNAL(creat)(KRF_SYS_PARMS) {
  return fault_table[KRF_LCG_NEXT() % NFAULTS](KRF_SYS_PARMSX);
}
