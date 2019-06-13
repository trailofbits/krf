#pragma once
// Linux specific definitions
#include "syscalls.h"

#define SAFE_WRITE(x) KRF_CR0_WRITE_UNLOCK(x)
#define LOG(...) printk(KERN_INFO __VA_ARGS__)
#define SYSCALL_TABLE sys_call_table

#define PERSONALITY() (current->personality)
#define PID() (current->pid)
#define UID() (current->cred->uid.val)
#define GID() (current->cred->gid.val)
