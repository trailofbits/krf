#pragma once
// Linux specific definitions
#include "syscalls.h"
#include "netlink.h"

#define KRF_SAFE_WRITE(x) KRF_CR0_WRITE_UNLOCK(x)
#define KRF_LOG(...)                                                                               \
  ({                                                                                               \
    char krf_log_msg_buf[KRF_NETLINK_BUF_SIZE];                                                    \
    printk(KERN_INFO __VA_ARGS__);                                                                 \
    int written = snprintf(krf_log_msg_buf, KRF_NETLINK_BUF_SIZE, __VA_ARGS__);                    \
    if (written < 0) {                                                                             \
      printk(KERN_WARNING "krf: snprintf formatting error\n");                                     \
    } else if (written >= KRF_NETLINK_BUF_SIZE) {                                                  \
      printk(KERN_WARNING "krf: truncated message\n");                                             \
      krf_netlink_broadcast(krf_log_msg_buf, KRF_NETLINK_BUF_SIZE);                                \
    } else {                                                                                       \
      krf_netlink_broadcast(krf_log_msg_buf, written + 1);                                         \
    }                                                                                              \
  })
#define KRF_SYSCALL_TABLE sys_call_table
#define KRF_TARGETING_PARMS current
#define KRF_EXTRACT_SYSCALL(x) (x)

typedef struct task_struct krf_ctx_t;
