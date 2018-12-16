#include "internal.h"

#define KRF_SYS_CALL write
#define KRF_SYS_PARMS unsigned int fd, const char __user *buf, size_t count
#define KRF_SYS_PARMSX fd, buf, count

/* TODO(ww): EPIPE, EAGAIN, EWOULDBLOCK, EDESTADDRREQ
 * fdget_pos is probably the right way to take a fd and turn it into
 * something that we can check the permissions/kind of (e.g. socket)
 */

DEFINE_FAULT(EBADF) {
  return -EBADF;
}

DEFINE_FAULT(EDQUOT) {
  return -EDQUOT;
}

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

DEFINE_FAULT(ENOSPC) {
  return -ENOSPC;
}

DEFINE_FAULT(EPERM) {
  return -EPERM;
}

static typeof(sys_write)(*fault_table[]) = {
    FAULT(EBADF), FAULT(EDQUOT), FAULT(EFAULT), FAULT(EFBIG),
    FAULT(EINTR), FAULT(EINVAL), FAULT(ENOSPC), FAULT(EPERM),
};

long KRF_DEFINE_INTERNAL(write)(KRF_SYS_PARMS) {
  return fault_table[KRF_LCG_NEXT() % NFAULTS](KRF_SYS_PARMSX);
}
