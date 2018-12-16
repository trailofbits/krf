#include "internal.h"

#define KRF_SYS_CALL read
#define KRF_SYS_PARMS unsigned int fd, char __user *buf, size_t count
#define KRF_SYS_PARMSX fd, buf, count

// EAGAIN / EWOULDBLOCK?

DEFINE_FAULT(EBADF) {
  return -EBADF;
}

DEFINE_FAULT(EFAULT) {
  return -EFAULT;
}

DEFINE_FAULT(EINTR) {
  return -EINTR;
}

DEFINE_FAULT(EINVAL) {
  return -EINVAL;
}

DEFINE_FAULT(EIO) {
  return -EIO;
}

// EISDIR?

static typeof(sys_read)(*fault_table[]) = {
    FAULT(EBADF), FAULT(EFAULT), FAULT(EINTR), FAULT(EINVAL), FAULT(EIO),
};

// Fault entrypoint.
long KRF_DEFINE_INTERNAL(read)(KRF_SYS_PARMS) {
  return fault_table[KRF_LCG_NEXT() % NFAULTS](KRF_SYS_PARMSX);
}
