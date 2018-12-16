#include "internal.h"

#define KRF_SYS_CALL fork
#define KRF_SYS_PARMS void
#define KRF_SYS_PARMSX

DEFINE_FAULT(EAGAIN) {
  return -EAGAIN;
}

DEFINE_FAULT(ENOMEM) {
  return -ENOMEM;
}

DEFINE_FAULT(ENOSYS) {
  return -ENOSYS;
}

DEFINE_FAULT(ERESTARTNOINTR) {
  return -ERESTARTNOINTR;
}

static typeof(sys_fork)(*fault_table[]) = {
    FAULT(EAGAIN),
    FAULT(ENOMEM),
    FAULT(ENOSYS),
    FAULT(ERESTARTNOINTR),
};

// Fault entrypoint.
long KRF_DEFINE_INTERNAL(fork)(KRF_SYS_PARMS) {
  return fault_table[KRF_LCG_NEXT() % NFAULTS](KRF_SYS_PARMSX);
}
