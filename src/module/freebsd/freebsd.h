#pragma once
// FreeBSD specific definitions
#include "syscalls.h"

#define KRF_SAFE_WRITE(x) x // ???
#define KRF_LOG(...) uprintf(__VA_ARGS__)
#define KRF_SYSCALL_TABLE sysent
#define KRF_TARGETING_PROTO struct thread *td
#define KRF_TARGETING_PARMS td
#define KRF_PERSONALITY(target) (td->td_proc->p_flag2 & (target))
#define KRF_PID(target)                                                                            \
  ({                                                                                               \
    struct proc *par = td->td_proc;                                                                \
    int ret = 0;                                                                                   \
    do {                                                                                           \
      if (par->p_pid == (target)) {                                                                \
        ret = 1;                                                                                   \
        break;                                                                                     \
      }                                                                                            \
    } while ((par = par->p_pptr));                                                                 \
    ret;                                                                                           \
  })
#define KRF_UID(target)                                                                            \
  (td->td_proc->p_ucred->cr_ruid ==                                                                \
   (target)) // Currently using real UID but could use effective UID (cr_uid)
#define KRF_GID(target) (td->td_proc->p_ucred->cr_rgid == (target))
#define KRF_EXTRACT_SYSCALL(x) ((x).sy_call)
