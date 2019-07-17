#pragma once
// FreeBSD specific definitions
#include "syscalls.h"

#define KRF_SAFE_WRITE(x) x // ???
#define KRF_LOG(...) uprintf(__VA_ARGS__)
#define KRF_SYSCALL_TABLE sysent
#define KRF_TARGETING_PARMS td
#define KRF_EXTRACT_SYSCALL(x) ((x).sy_call)
typedef struct thread krf_ctx_t;
