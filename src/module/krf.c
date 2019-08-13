#include "config.h"
#include "krf.h"
#ifdef LINUX
#include "linux/linux.h"
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/kallsyms.h>
#include <linux/unistd.h>
#include <linux/proc_fs.h>
#include <linux/string.h>
#endif

#ifdef __FreeBSD__
#include <sys/types.h>
#include <sys/syscall.h>
#include <sys/sysproto.h>
#include <sys/sysent.h>
#include "freebsd/freebsd.h"
#endif

void krf_flush_table(void) {
  int nr;
  for (nr = 0; nr < KRF_NR_SYSCALLS; nr++) {
    if (krf_sys_call_table[nr]) {
      KRF_SAFE_WRITE({ KRF_EXTRACT_SYSCALL(KRF_SYSCALL_TABLE[nr]) = krf_sys_call_table[nr]; });
    }
  }
}

int control_file_handler(unsigned int sys_num) {
  if (sys_num >= KRF_NR_SYSCALLS) {
    KRF_LOG("krf: flushing all faulty syscalls\n");
    krf_flush_table();
  } else if (krf_faultable_table[sys_num] != NULL) {
    KRF_SAFE_WRITE(
        { KRF_EXTRACT_SYSCALL(KRF_SYSCALL_TABLE[sys_num]) = krf_faultable_table[sys_num]; });
  } else {
    // Valid syscall, but not supported by KRF
    KRF_LOG("krf: user requested faulting of unsupported slot %u\n", sys_num);
    return -EOPNOTSUPP;
  }
  return 0;
}

void targeting_file_read_handler(char *buf) {
  size_t offset = 0;
  unsigned int current_mode;
  for (current_mode = 0; current_mode < KRF_T_NUM_MODES; current_mode++) {
    if ((krf_target_options.mode_mask & (1 << current_mode)) && (offset < KRF_PROCFS_MAX_SIZE)) {
      offset += sprintf(buf + offset, "%u %u\n", current_mode,
                        krf_target_options.target_data[current_mode]);
    }
  }
}

int targeting_file_write_handler(unsigned int mode, unsigned int data) {
  if ((mode == 0) && (data == 0)) { // If both arguments are zero, remove all targeting
    krf_target_options.mode_mask = 0;
    KRF_LOG("krf: flushing all targeting options\n");
  } else {
    if (mode >= KRF_T_NUM_MODES) {
      return -EINVAL;
    }
    krf_target_options.mode_mask |= (1 << mode);
    krf_target_options.target_data[mode] = data;
  }
  return 0;
}
