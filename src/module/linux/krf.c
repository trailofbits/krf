#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/kallsyms.h>
#include <linux/unistd.h>
#include <linux/proc_fs.h>
#include <linux/string.h>
#include <linux/version.h>

#include "../config.h"
#include "../krf.h"
#include "syscalls.h"
#include "netlink.h"

#define KRF_VERSION "0.0.1"

MODULE_LICENSE("GPL");
MODULE_AUTHOR("William Woodruff <william@yossarian.net>");
MODULE_DESCRIPTION("A Kernelspace Randomized Faulter");

// Kernels 5.6 and newer: procfs uses `struct proc_ops` instead of `struct file_operations`.
#if LINUX_VERSION_CODE >= KERNEL_VERSION(5, 6, 0)
#define HAVE_PROC_OPS
#endif

// Kernels 5.7 and newer: kallsyms_lookup_name has been unexported for Google reasons (tm),
// so we need to use kprobes to grab its address.
// See: https://github.com/xcellerator/linux_kernel_hacking
#if LINUX_VERSION_CODE >= KERNEL_VERSION(5, 7, 0)
#define KALLSYMS_LOOKUP_NAME_UNEXPORTED
#include <linux/kprobes.h>
static struct kprobe kp = {.symbol_name = "kallsyms_lookup_name"};
#endif

static int krf_init(void);
static void krf_teardown(void);
static ssize_t rng_state_file_read(struct file *, char __user *, size_t, loff_t *);
static ssize_t rng_state_file_write(struct file *, const char __user *, size_t, loff_t *);
static ssize_t probability_file_read(struct file *, char __user *, size_t, loff_t *);
static ssize_t probability_file_write(struct file *, const char __user *, size_t, loff_t *);
static ssize_t control_file_write(struct file *, const char __user *, size_t, loff_t *);
static ssize_t log_faults_file_read(struct file *, char __user *, size_t, loff_t *);
static ssize_t log_faults_file_write(struct file *, const char __user *, size_t, loff_t *);
static ssize_t targeting_file_read(struct file *, char __user *, size_t, loff_t *);
static ssize_t targeting_file_write(struct file *, const char __user *, size_t, loff_t *);

static struct proc_dir_entry *krf_dir;

#ifdef HAVE_PROC_OPS
static const struct proc_ops rng_state_file_ops = {
    .proc_read = rng_state_file_read,
    .proc_write = rng_state_file_write,
};

static const struct proc_ops probability_file_ops = {
    .proc_read = probability_file_read,
    .proc_write = probability_file_write,
};

static const struct proc_ops control_file_ops = {
    .proc_write = control_file_write,
};

static const struct proc_ops log_faults_file_ops = {
    .proc_read = log_faults_file_read,
    .proc_write = log_faults_file_write,
};

static const struct proc_ops targeting_file_ops = {
    .proc_read = targeting_file_read,
    .proc_write = targeting_file_write,
};
#else
static const struct file_operations rng_state_file_ops = {
    .owner = THIS_MODULE,
    .read = rng_state_file_read,
    .write = rng_state_file_write,
};

static const struct file_operations probability_file_ops = {
    .owner = THIS_MODULE,
    .read = probability_file_read,
    .write = probability_file_write,
};

static const struct file_operations control_file_ops = {
    .owner = THIS_MODULE,
    .write = control_file_write,
};

static const struct file_operations log_faults_file_ops = {
    .owner = THIS_MODULE,
    .read = log_faults_file_read,
    .write = log_faults_file_write,
};

static const struct file_operations targeting_file_ops = {
    .owner = THIS_MODULE,
    .read = targeting_file_read,
    .write = targeting_file_write,
};
#endif

int init_module(void) {
  int ret;

  if ((ret = krf_init()) != 0) {
    printk(KERN_ERR "krf_init failed with %d\n", ret);
    return ret;
  }

#ifdef KRF_CODEGEN
#include "krf.gen.x"
#endif

  printk(KERN_INFO "krf " KRF_VERSION " loaded\n");

  return 0;
}

void cleanup_module(void) {
  krf_teardown();

  printk(KERN_INFO "krf " KRF_VERSION " unloaded\n");
}

static int krf_init(void) {
#ifdef KALLSYMS_LOOKUP_NAME_UNEXPORTED
  typedef unsigned long (*kallsyms_lookup_name_t)(const char *name);
  kallsyms_lookup_name_t kallsyms_lookup_name;
  if (register_kprobe(&kp) < 0) {
    printk(KERN_ERR "krf couldn't register a kprobe to sniff kallsyms_lookup_name\n");
  }
  kallsyms_lookup_name = (kallsyms_lookup_name_t)kp.addr;
  unregister_kprobe(&kp);
#endif

  if (setup_netlink_socket() < 0) {
    return -1;
  }

  sys_call_table = (void *)kallsyms_lookup_name("sys_call_table");

  if (sys_call_table == NULL) {
    printk(KERN_ERR "krf couldn't load the syscall table\n");
    return -2;
  }

  memcpy(krf_sys_call_table, sys_call_table, KRF_NR_SYSCALLS * sizeof(unsigned long *));

  krf_dir = proc_mkdir(KRF_PROC_DIR, NULL);

  if (krf_dir == NULL) {
    printk(KERN_ERR "krf couldn't create /proc/" KRF_PROC_DIR);
    return -2;
  }

  if (proc_create(KRF_RNG_STATE_FILENAME, 644, krf_dir, &rng_state_file_ops) == NULL ||
      proc_create(KRF_PROBABILITY_FILENAME, 644, krf_dir, &probability_file_ops) == NULL ||
      proc_create(KRF_CONTROL_FILENAME, 644, krf_dir, &control_file_ops) == NULL ||
      proc_create(KRF_LOG_FAULTS_FILENAME, 644, krf_dir, &log_faults_file_ops) == NULL ||
      proc_create(KRF_TARGETING_FILENAME, 644, krf_dir, &targeting_file_ops) == NULL) {
    printk(KERN_ERR "krf couldn't create /proc entries\n");
    return -3;
  }

  return 0;
}

static void krf_teardown(void) {
  krf_flush_table();
  remove_proc_subtree(KRF_PROC_DIR, NULL);
  destroy_netlink_socket();
}

static ssize_t rng_state_file_read(struct file *f, char __user *ubuf, size_t size, loff_t *off) {
  char buf[KRF_PROCFS_MAX_SIZE + 1] = {0};
  size_t buflen = 0;

  sprintf(buf, "%u\n", krf_rng_state);
  buflen = strnlen(buf, KRF_PROCFS_MAX_SIZE);

  if (*off > 0 || size < buflen) {
    return 0;
  }

  if (copy_to_user(ubuf, buf, buflen)) {
    return -EFAULT;
  }

  *off = buflen;
  return buflen;
}

static ssize_t rng_state_file_write(struct file *f, const char __user *ubuf, size_t size,
                                    loff_t *off) {
  char buf[KRF_PROCFS_MAX_SIZE + 1] = {0};
  size_t buflen = 0;

  if (size > KRF_PROCFS_MAX_SIZE) {
    size = KRF_PROCFS_MAX_SIZE;
  }

  if (copy_from_user(buf, ubuf, size)) {
    return -EFAULT;
  }

  if (kstrtouint(buf, 0, &krf_rng_state) < 0) {
    return -EINVAL;
  }

  buflen = strnlen(buf, KRF_PROCFS_MAX_SIZE);

  *off = buflen;
  return buflen;
}

static ssize_t probability_file_read(struct file *f, char __user *ubuf, size_t size, loff_t *off) {
  char buf[KRF_PROCFS_MAX_SIZE + 1] = {0};
  size_t buflen = 0;

  sprintf(buf, "%u\n", krf_probability);
  buflen = strnlen(buf, KRF_PROCFS_MAX_SIZE);

  if (*off > 0 || size < buflen) {
    return 0;
  }

  if (copy_to_user(ubuf, buf, buflen)) {
    return -EFAULT;
  }

  *off = buflen;
  return buflen;
}

static ssize_t probability_file_write(struct file *f, const char __user *ubuf, size_t size,
                                      loff_t *off) {
  char buf[KRF_PROCFS_MAX_SIZE + 1] = {0};
  size_t buflen = 0;

  if (size > KRF_PROCFS_MAX_SIZE) {
    size = KRF_PROCFS_MAX_SIZE;
  }

  if (copy_from_user(buf, ubuf, size)) {
    return -EFAULT;
  }

  if (kstrtouint(buf, 0, &krf_probability) < 0) {
    return -EINVAL;
  }

  buflen = strnlen(buf, KRF_PROCFS_MAX_SIZE);

  *off = buflen;
  return buflen;
}

static ssize_t control_file_write(struct file *f, const char __user *ubuf, size_t size,
                                  loff_t *off) {
  char buf[KRF_PROCFS_MAX_SIZE + 1] = {0};
  size_t buflen = 0;
  unsigned int sys_num = KRF_NR_SYSCALLS;

  if (size > KRF_PROCFS_MAX_SIZE) {
    size = KRF_PROCFS_MAX_SIZE;
  }

  if (copy_from_user(buf, ubuf, size)) {
    return -EFAULT;
  }

  if (kstrtouint(buf, 0, &sys_num) < 0) {
    return -EINVAL;
  }

  if (control_file_handler(sys_num) < 0) {
    return -EOPNOTSUPP;
  }

  buflen = strnlen(buf, KRF_PROCFS_MAX_SIZE);

  *off = buflen;
  return buflen;
}

static ssize_t log_faults_file_read(struct file *f, char __user *ubuf, size_t size, loff_t *off) {
  char buf[KRF_PROCFS_MAX_SIZE + 1] = {0};
  size_t buflen = 0;

  sprintf(buf, "%u\n", krf_log_faults);
  buflen = strnlen(buf, KRF_PROCFS_MAX_SIZE);

  if (*off > 0 || size < buflen) {
    return 0;
  }

  if (copy_to_user(ubuf, buf, buflen)) {
    return -EFAULT;
  }

  *off = buflen;
  return buflen;
}

static ssize_t log_faults_file_write(struct file *f, const char __user *ubuf, size_t size,
                                     loff_t *off) {
  char buf[KRF_PROCFS_MAX_SIZE + 1] = {0};
  size_t buflen = 0;

  if (size > KRF_PROCFS_MAX_SIZE) {
    size = KRF_PROCFS_MAX_SIZE;
  }

  if (copy_from_user(buf, ubuf, size)) {
    return -EFAULT;
  }

  if (kstrtouint(buf, 0, &krf_log_faults) < 0) {
    return -EINVAL;
  }

  krf_log_faults = !!krf_log_faults;

  buflen = strnlen(buf, KRF_PROCFS_MAX_SIZE);

  *off = buflen;
  return buflen;
}

static ssize_t targeting_file_read(struct file *f, char __user *ubuf, size_t size, loff_t *off) {
  char buf[KRF_PROCFS_MAX_SIZE + 1] = {0};
  size_t buflen = 0;

  targeting_file_read_handler(buf);

  buflen = strnlen(buf, KRF_PROCFS_MAX_SIZE);

  if (*off > 0 || size < buflen) {
    return 0;
  }

  if (copy_to_user(ubuf, buf, buflen)) {
    return -EFAULT;
  }

  *off = buflen;
  return buflen;
}

static ssize_t targeting_file_write(struct file *f, const char __user *ubuf, size_t size,
                                    loff_t *off) {
  char buf[KRF_PROCFS_MAX_SIZE + 1] = {0};
  size_t buflen = 0;
  krf_target_mode_t mode;
  unsigned int data;

  if (size > KRF_PROCFS_MAX_SIZE) {
    size = KRF_PROCFS_MAX_SIZE;
  }

  if (copy_from_user(buf, ubuf, size)) {
    return -EFAULT;
  }

  if (sscanf(buf, "%u %u", &mode, &data) != 2) {
    return -EINVAL;
  }

  if (targeting_file_write_handler(mode, data) < 0) {
    return -EINVAL;
  }

  buflen = strnlen(buf, KRF_PROCFS_MAX_SIZE);

  *off = buflen;
  return buflen;
}
