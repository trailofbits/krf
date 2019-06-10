#pragma once

#include <linux/syscalls.h>
#include <linux/preempt.h>
#include <asm/asm-offsets.h> /* for NR_syscalls */

#if !defined(NR_syscalls) || NR_syscalls <= 0
#error "undefined or bizarrely defined NR_syscalls"
#endif

#define KRF_NR_SYSCALLS (NR_syscalls)

#define KRF_CR0_WRITE_UNLOCK(x)                                                                    \
  do {                                                                                             \
    unsigned long __cr0;                                                                           \
    preempt_disable();                                                                             \
    __cr0 = read_cr0() & (~X86_CR0_WP);                                                            \
    BUG_ON(unlikely((__cr0 & X86_CR0_WP)));                                                        \
    write_cr0(__cr0);                                                                              \
    x;                                                                                             \
    __cr0 = read_cr0() | X86_CR0_WP;                                                               \
    BUG_ON(unlikely(!(__cr0 & X86_CR0_WP)));                                                       \
    write_cr0(__cr0);                                                                              \
    preempt_enable();                                                                              \
  } while (0)

#define KRF_DEFINE_PROTO(sys) asmlinkage typeof(sys_##sys) krf_sys_##sys

#define KRF_DEFINE(sys) asmlinkage krf_sys_##sys

#define KRF_TARGETED()                                                                             \
  ({                                                                                               \
    int targeted = 1;                                                                              \
    size_t i = 0;                                                                                  \
    for (; i < KRF_T_NUM_MODES; i++) {                                                             \
      if (targeted == 0)                                                                           \
        break;                                                                                     \
                                                                                                   \
      if (krf_target_options.mode_mask & (1 << i)) {                                               \
        switch (i) {                                                                               \
        case KRF_T_MODE_PERSONALITY:                                                               \
          if ((current->personality & krf_target_options.target_data[i]) != 0)                     \
            targeted++;                                                                            \
          else                                                                                     \
            targeted = 0;                                                                          \
          break;                                                                                   \
        case KRF_T_MODE_PID:                                                                       \
          if (current->pid == krf_target_options.target_data[i])                                   \
            targeted++;                                                                            \
          else                                                                                     \
            targeted = 0;                                                                          \
          break;                                                                                   \
        case KRF_T_MODE_UID:                                                                       \
          if (current->cred->uid.val == krf_target_options.target_data[i])                         \
            targeted++;                                                                            \
          else                                                                                     \
            targeted = 0;                                                                          \
          break;                                                                                   \
        case KRF_T_MODE_GID:                                                                       \
          if (current->cred->gid.val == krf_target_options.target_data[i])                         \
            targeted++;                                                                            \
          else                                                                                     \
            targeted = 0;                                                                          \
          break;                                                                                   \
        }                                                                                          \
      }                                                                                            \
    }                                                                                              \
    (targeted & (~1));                                                                             \
  })

/* A table of pointers to faulty syscalls.
 */
extern unsigned long *krf_faultable_table[KRF_NR_SYSCALLS];

/* A backup of the real syscall table, prior to modification.
 */
extern unsigned long *krf_sys_call_table[KRF_NR_SYSCALLS];

/* The real syscall table, which may or may not be modified at any point.
 */
extern unsigned long **sys_call_table;

#ifdef KRF_CODEGEN
#include "syscalls.gen.h"
#endif
