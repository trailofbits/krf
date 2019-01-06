#include "config.h"
#include "syscalls.h"
#include "syscalls/internal.h"

unsigned long *krf_sys_call_table[KRF_NR_SYSCALLS] = {};
unsigned long **sys_call_table = NULL;

long KRF_DEFINE(chdir)(const char __user *filename) {
  typeof(sys_chdir) *real_chdir = (void *)krf_sys_call_table[__NR_chdir];

  if (KRF_TARGETED() && (KRF_LCG_NEXT() % krf_probability) == 0) {
    return KRF_SYS_INTERNAL(chdir)(filename);
  } else {
    return real_chdir(filename);
  }
}

long KRF_DEFINE(fchdir)(unsigned int fd) {
  typeof(sys_fchdir) *real_fchdir = (void *)krf_sys_call_table[__NR_fchdir];

  if (KRF_TARGETED() && (KRF_LCG_NEXT() % krf_probability) == 0) {
    return KRF_SYS_INTERNAL(fchdir)(fd);
  } else {
    return real_fchdir(fd);
  }
}

long KRF_DEFINE(mkdir)(const char __user *pathname, umode_t mode) {
  typeof(sys_mkdir) *real_mkdir = (void *)krf_sys_call_table[__NR_mkdir];

  if (KRF_TARGETED() && (KRF_LCG_NEXT() % krf_probability) == 0) {
    return KRF_SYS_INTERNAL(mkdir)(pathname, mode);
  } else {
    return real_mkdir(pathname, mode);
  }
}

long KRF_DEFINE(rmdir)(const char __user *pathname) {
  typeof(sys_rmdir) *real_rmdir = (void *)krf_sys_call_table[__NR_rmdir];

  if (KRF_TARGETED() && (KRF_LCG_NEXT() % krf_probability) == 0) {
    return KRF_SYS_INTERNAL(rmdir)(pathname);
  } else {
    return real_rmdir(pathname);
  }
}

long KRF_DEFINE(creat)(const char __user *pathname, umode_t mode) {
  typeof(sys_creat) *real_creat = (void *)krf_sys_call_table[__NR_creat];

  if (KRF_TARGETED() && (KRF_LCG_NEXT() % krf_probability) == 0) {
    return KRF_SYS_INTERNAL(creat)(pathname, mode);
  } else {
    return real_creat(pathname, mode);
  }
}

long KRF_DEFINE(link)(const char __user *oldname, const char __user *newname) {
  typeof(sys_link) *real_link = (void *)krf_sys_call_table[__NR_link];

  if (KRF_TARGETED() && (KRF_LCG_NEXT() % krf_probability) == 0) {
    return KRF_SYS_INTERNAL(link)(oldname, newname);
  } else {
    return real_link(oldname, newname);
  }
}

#ifdef KRF_CODEGEN
#include "syscalls.gen.x"
#endif
