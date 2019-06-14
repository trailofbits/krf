#pragma once
// FreeBSD specific definitions
#include "syscalls.h"

#define SAFE_WRITE(x) x // ???
#define LOG(...) uprintf(__VA_ARGS__)
#define SYSCALL_TABLE sysent
#define TARGETING_PROTO struct thread *td
#define TARGETING_PARMS td
#define PERSONALITY() (td->td_proc->p_flag2)
#define PID() (td->td_proc->p_pid)
#define UID()                                                                                      \
  (td->td_proc->p_ucred->cr_ruid) // Currently using real UID but could use effective UID (cr_uid)
#define GID() (td->td_proc->p_ucred->cr_rgid)
#define KRF_EXTRACT_SYSCALL(x) ((x).sy_call)
