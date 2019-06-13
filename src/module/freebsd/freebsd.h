#pragma once
// FreeBSD specific definitions
#include "syscalls.h"

#define SAFE_WRITE(x) x // ???
#define LOG(...) uprintf(__VA_ARGS__)
#define SYSCALL_TABLE sysent

#define PERSONALITY() (td->td_proc->p_flag2)
#define PID() -1 // ???
#define UID() -1 // ???
#define GID() -1 // ???
