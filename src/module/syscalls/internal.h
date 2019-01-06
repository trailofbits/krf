#pragma once

#include <linux/syscalls.h>

#include "../config.h"

#define KRF_LCG_NEXT() (krf_rng_state = krf_mulberry32())

#define KRF_SYS_INTERNAL(sys) krf_sys_internal_##sys
#define KRF_DEFINE_INTERNAL_PROTO(sys) typeof(sys_##sys) KRF_SYS_INTERNAL(sys)
#define KRF_DEFINE_INTERNAL(sys) KRF_SYS_INTERNAL(sys)

/* Individual syscall files (read.c, write.c) provide these.
 */
#undef KRF_SYS_CALL
#undef KRF_SYS_PARMS
#undef KRF_SYS_PARMSX

#define FAULT(x) KRF_SYS_INTERNAL(_##KRF_SYS_CALL_##x)
#define FAULTX(x) FAULT(x)(KRF_SYS_PARMSX)
#define DEFINE_FAULT(x) static long KRF_DEFINE_INTERNAL(_##KRF_SYS_CALL_##x)(KRF_SYS_PARMS)
#define NFAULTS (sizeof(fault_table) / sizeof(fault_table[0]))

/* Cribbed from the public domain impl:
 * https://gist.github.com/tommyettinger/46a874533244883189143505d203312c
 *
 * TODO(ww): 64 bit would probably be faster; use Thrust instead?
 */
static __inline unsigned int krf_mulberry32(void) {
  unsigned int z = krf_rng_state += 0x6D2B79F5;
  z = (z ^ z >> 15) * (1 | z);
  z ^= z + (z ^ z >> 7) * (61 | z);
  return z ^ z >> 14;
}

KRF_DEFINE_INTERNAL_PROTO(openat);
KRF_DEFINE_INTERNAL_PROTO(chdir);
KRF_DEFINE_INTERNAL_PROTO(fchdir);
KRF_DEFINE_INTERNAL_PROTO(mkdir);
KRF_DEFINE_INTERNAL_PROTO(rmdir);
KRF_DEFINE_INTERNAL_PROTO(creat);
KRF_DEFINE_INTERNAL_PROTO(link);

#ifdef KRF_CODEGEN
#include "internal.gen.h"
#endif
