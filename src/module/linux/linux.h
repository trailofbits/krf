#pragma once
// Linux specific definitions
#include "syscalls.h"

#define KRF_SAFE_WRITE(x) KRF_CR0_WRITE_UNLOCK(x)
#define KRF_LOG(...) printk(KERN_INFO __VA_ARGS__)
#define KRF_SYSCALL_TABLE sys_call_table
#define KRF_TARGETING_PROTO void
#define KRF_TARGETING_PARMS
#define KRF_PERSONALITY() (current->personality)
#define KRF_PID(target) (current->pid == (target))
#define KRF_UID() (current->cred->uid.val)
#define KRF_GID() (current->cred->gid.val)
#define KRF_EXTRACT_SYSCALL(x) (x)
