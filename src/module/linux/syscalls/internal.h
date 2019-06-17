#pragma once

#ifdef LINUX
#include <linux/syscalls.h>
#endif

#include "../../config.h"
#include "../../targeting.h"

#define KRF_RNG_NEXT() (krf_rng_state = krf_mulberry32())

/* Individual syscall files (read.c, write.c) provide these.
 */
#undef KRF_SYS_CALL
#undef KRF_SYS_PARMS
#undef KRF_SYS_PARMSX

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

#ifdef KRF_CODEGEN
#include "internal.gen.h"
#endif
