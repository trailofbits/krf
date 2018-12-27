#include "config.h"
#include "syscalls.h"
#include "syscalls/internal.h"

unsigned long *krf_sys_call_table[KRF_NR_SYSCALLS] = {};
unsigned long **sys_call_table = NULL;

long KRF_DEFINE(read)(unsigned int fd, char __user *buf, size_t count) {
  typeof(sys_read) *real_read = (void *)krf_sys_call_table[__NR_read];

  if (KRF_TARGETED() && (KRF_LCG_NEXT() % krf_probability) == 0) {
    return KRF_SYS_INTERNAL(read)(fd, buf, count);
  } else {
    return real_read(fd, buf, count);
  }
}

long KRF_DEFINE(write)(unsigned int fd, const char __user *buf, size_t count) {
  typeof(sys_write) *real_write = (void *)krf_sys_call_table[__NR_write];

  if (KRF_TARGETED() && (KRF_LCG_NEXT() % krf_probability) == 0) {
    return KRF_SYS_INTERNAL(write)(fd, buf, count);
  } else {
    return real_write(fd, buf, count);
  }
}

long KRF_DEFINE(open)(const char __user *filename, int flags, umode_t mode) {
  typeof(sys_open) *real_open = (void *)krf_sys_call_table[__NR_open];

  if (KRF_TARGETED() && (KRF_LCG_NEXT() % krf_probability) == 0) {
    return KRF_SYS_INTERNAL(open)(filename, flags, mode);
  } else {
    return real_open(filename, flags, mode);
  }
}

long KRF_DEFINE(close)(unsigned int fd) {
  typeof(sys_close) *real_close = (void *)krf_sys_call_table[__NR_close];

  if (KRF_TARGETED() && (KRF_LCG_NEXT() % krf_probability) == 0) {
    return KRF_SYS_INTERNAL(close)(fd);
  } else {
    return real_close(fd);
  }
}

long KRF_DEFINE(clone)(unsigned long clone_flags, unsigned long newsp, int __user *parent_tidptr,
                       int __user *child_tidptr, unsigned long tls) {
  typeof(sys_clone) *real_clone = (void *)krf_sys_call_table[__NR_clone];

  if (KRF_TARGETED() && (KRF_LCG_NEXT() % krf_probability) == 0) {
    return KRF_SYS_INTERNAL(clone)(clone_flags, newsp, parent_tidptr, child_tidptr, tls);
  } else {
    return real_clone(clone_flags, newsp, parent_tidptr, child_tidptr, tls);
  }
}

long KRF_DEFINE(fork)(void) {
  typeof(sys_fork) *real_fork = (void *)krf_sys_call_table[__NR_fork];

  if (KRF_TARGETED() && (KRF_LCG_NEXT() % krf_probability) == 0) {
    return KRF_SYS_INTERNAL(fork)();
  } else {
    return real_fork();
  }
}

long KRF_DEFINE(openat)(int dfd, const char __user *filename, int flags, umode_t mode) {
  typeof(sys_openat) *real_openat = (void *)krf_sys_call_table[__NR_openat];

  if (KRF_TARGETED() && (KRF_LCG_NEXT() % krf_probability) == 0) {
    return KRF_SYS_INTERNAL(openat)(dfd, filename, flags, mode);
  } else {
    return real_openat(dfd, filename, flags, mode);
  }
}

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
