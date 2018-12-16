#include "internal.h"

#define KRF_SYS_CALL close
#define KRF_SYS_PARMS unsigned int fd
#define KRF_SYS_PARMSX fd

// EAGAIN / EWOULDBLOCK?

DEFINE_FAULT(EBADF) {
  return -EBADF;
}

DEFINE_FAULT(EINTR) {
  return -EINTR;
}

DEFINE_FAULT(EIO) {
  return -EIO;
}

DEFINE_FAULT(ENOSPC) {
  return -ENOSPC;
}

DEFINE_FAULT(EDQUOT) {
  return -EDQUOT;
}

// EISDIR?

static typeof(sys_close)(*fault_table[]) = {
    FAULT(EBADF), FAULT(EINTR), FAULT(EIO), FAULT(ENOSPC), FAULT(EDQUOT),
};

// Fault entrypoint.
long KRF_DEFINE_INTERNAL(close)(KRF_SYS_PARMS) {
  return fault_table[KRF_LCG_NEXT() % NFAULTS](KRF_SYS_PARMSX);
}
