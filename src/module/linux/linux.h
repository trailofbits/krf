#pragma once
// Linux specific definitions
#include "syscalls.h"
#include "netlink.h"

#define KRF_SAFE_WRITE(x) KRF_CR0_WRITE_UNLOCK(x)
#define KRF_LOG(...)                                                                               \
  ({                                                                                               \
    int krf_snprintf_ret_val;                                                                      \
    get_cpu_var(krf_log_msg_buf);                                                                  \
    printk(KERN_INFO __VA_ARGS__);                                                                 \
    krf_snprintf_ret_val = snprintf(krf_log_msg_buf, KRF_NETLINK_BUF_SIZE, __VA_ARGS__);           \
    if (krf_snprintf_ret_val < 0) {                                                                \
      printk(KERN_WARNING "snprintf formatting error");                                            \
    } else {                                                                                       \
      krf_netlink_broadcast(krf_log_msg_buf, krf_snprintf_ret_val + 1);                            \
    }                                                                                              \
    put_cpu_var(krf_log_msg_buf);                                                                  \
  })
#define KRF_SYSCALL_TABLE sys_call_table
#define KRF_TARGETING_PARMS current
#define KRF_EXTRACT_SYSCALL(x) (x)

typedef struct task_struct krf_ctx_t;

DECLARE_PER_CPU(char[KRF_NETLINK_BUF_SIZE], krf_log_msg_buf);
