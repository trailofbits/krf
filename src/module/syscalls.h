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
  (((current->personality & krf_personality) != 0) &&                                              \
   ((KRF_PID_TARGETING()) ? (current->pid == krf_pid_target) : (1)) &&                             \
   ((KRF_UID_TARGETING()) ? (current->cred->uid.val == krf_uid_target) : (1)) &&                   \
   ((KRF_GID_TARGETING()) ? (current->cred->gid.val == krf_gid_target) : (1)))

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
