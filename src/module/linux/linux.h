#pragma once
// Linux specific definitions
#include "syscalls.h"

#define KRF_SAFE_WRITE(x) KRF_CR0_WRITE_UNLOCK(x)
#define KRF_LOG(...) printk(KERN_INFO __VA_ARGS__)
#define KRF_SYSCALL_TABLE sys_call_table
#define KRF_TARGETING_PROTO void
#define KRF_TARGETING_PARMS
#define KRF_PERSONALITY(target) (current->personality & (target))
#define KRF_PID(target) (current->pid == (target))
#define KRF_UID(target) (current->cred->uid.val == (target))
#define KRF_GID(target) (current->cred->gid.val == (target))
#define KRF_EXTRACT_SYSCALL(x) (x)
