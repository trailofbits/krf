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

#include "syscalls.gen.h"
