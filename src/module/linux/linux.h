#pragma once
// Linux specific definitions
#include "syscalls.h"

#define KRF_SAFE_WRITE(x) KRF_CR0_WRITE_UNLOCK(x)
#define KRF_LOG(...) printk(KERN_INFO __VA_ARGS__)
#define KRF_SYSCALL_TABLE sys_call_table
#define KRF_TARGETING_PARMS current
#define KRF_EXTRACT_SYSCALL(x) (x)
typedef struct task_struct krf_ctx_t;

bool krf_personality(unsigned int target, krf_ctx_t *context);
bool krf_pid(pid_t target, krf_ctx_t *context);
bool krf_uid(unsigned int target, krf_ctx_t *context);
bool krf_gid(unsigned int target, krf_ctx_t *context);
bool krf_file(unsigned int target, krf_ctx_t *context);
