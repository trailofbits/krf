#pragma once
// Linux specific definitions
#include "syscalls.h"
#include "netlink.h"

#define KRF_SAFE_WRITE(x) KRF_CR0_WRITE_UNLOCK(x)
#define KRF_LOG(...)                                                                               \
  ({                                                                                               \
    char buf[KRF_NETLINK_BUF_SIZE]; /* max message size */                                         \
    printk(KERN_INFO __VA_ARGS__);                                                                 \
    snprintf(buf, KRF_NETLINK_BUF_SIZE, __VA_ARGS__);                                              \
    krf_netlink_broadcast(buf, strlen(buf) + 1);                                                   \
  })
#define KRF_SYSCALL_TABLE sys_call_table
#define KRF_TARGETING_PARMS current
#define KRF_EXTRACT_SYSCALL(x) (x)
typedef struct task_struct krf_ctx_t;
