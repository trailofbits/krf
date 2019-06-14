#pragma once

#if !defined(SYS_MAXSYSCALL) || SYS_MAXSYSCALL <= 0
#error "undefined or bizarrely defined SYS_MAXSYSCALL"
#endif

#define KRF_NR_SYSCALLS SYS_MAXSYSCALL
#define KRF_MAX_SYSCALL SYS_MAXSYSCALL

struct sysent;
extern struct sysent sysent[];
extern struct sysent krf_faultable_table[KRF_MAX_SYSCALL];
extern struct sysent krf_sys_call_table[KRF_MAX_SYSCALL];

#define KRF_TARGETED() ((td->td_proc->p_flag2 & krf_personality) != 0)

#define KRF_DEFINE_PROTO(sys) __typeof(sys_##sys) krf_sys_##sys
#define KRF_DEFINE(sys) krf_sys_##sys

#include "syscalls.gen.h"
