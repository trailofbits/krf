#pragma once
// Linux specific definitions
#include "syscalls.h"

#define SAFE_WRITE(x) KRF_CR0_WRITE_UNLOCK(x)
#define LOG(...) printk(KERN_INFO __VA_ARGS__)
#define SYSCALL_TABLE sys_call_table
#define TARGETING_PROTO void
#define TARGETING_PARAMS void
#define PERSONALITY() (current->personality)
#define PID() (current->pid)
#define UID() (current->cred->uid.val)
#define GID() (current->cred->gid.val)
#define KRF_EXTRACT_SYSCALL(x) (x)
