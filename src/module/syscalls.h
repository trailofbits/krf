#pragma once

#include <linux/syscalls.h>
#include <linux/preempt.h>

/* I might just be dumb, but I can't find __NR_syscalls on x64.
 * This just cheeses it (and is probably still overkill, since
 * we probably won't ever want to fault any of the higher syscalls).
 */
#define KRF_NR_SYSCALLS (__NR_statx + 1)

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

#define KRF_TARGETED() (current->personality == krf_personality)

extern unsigned long *krf_sys_call_table[KRF_NR_SYSCALLS];
extern unsigned long **sys_call_table;

#ifdef KRF_CODEGEN
#include "syscalls.gen.h"
#endif
