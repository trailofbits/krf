#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/kallsyms.h>
#include <linux/unistd.h>
#include <linux/proc_fs.h>
#include <linux/string.h>

#include "../config.h"
#include "../krf.h"
#include "syscalls.h"

#define KRF_VERSION "0.0.1"

MODULE_LICENSE("GPL");
MODULE_AUTHOR("William Woodruff <william@yossarian.net>");
MODULE_DESCRIPTION("A Kernelspace Randomized Faulter");

static int krf_init(void);
// static void krf_flush_table(void);
static void krf_teardown(void);
static ssize_t rng_state_file_read(struct file *, char __user *, size_t, loff_t *);
static ssize_t rng_state_file_write(struct file *, const char __user *, size_t, loff_t *);
static ssize_t personality_file_read(struct file *, char __user *, size_t, loff_t *);
static ssize_t personality_file_write(struct file *, const char __user *, size_t, loff_t *);
static ssize_t probability_file_read(struct file *, char __user *, size_t, loff_t *);
static ssize_t probability_file_write(struct file *, const char __user *, size_t, loff_t *);
static ssize_t control_file_write(struct file *, const char __user *, size_t, loff_t *);
static ssize_t log_faults_file_read(struct file *, char __user *, size_t, loff_t *);
static ssize_t log_faults_file_write(struct file *, const char __user *, size_t, loff_t *);
static ssize_t targeting_file_read(struct file *, char __user *, size_t, loff_t *);
static ssize_t targeting_file_write(struct file *, const char __user *, size_t, loff_t *);

static struct proc_dir_entry *krf_dir;

static const struct file_operations rng_state_file_ops = {
    .owner = THIS_MODULE,
    .read = rng_state_file_read,
    .write = rng_state_file_write,
};

static const struct file_operations personality_file_ops = {
    .owner = THIS_MODULE,
    .read = personality_file_read,
    .write = personality_file_write,
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
  sys_call_table = (void *)kallsyms_lookup_name("sys_call_table");

  if (sys_call_table == NULL) {
    printk(KERN_ERR "krf couldn't load the syscall table\n");
    return -1;
  }

  memcpy(krf_sys_call_table, sys_call_table, KRF_NR_SYSCALLS * sizeof(unsigned long *));

  krf_dir = proc_mkdir(KRF_PROC_DIR, NULL);

  if (krf_dir == NULL) {
    printk(KERN_ERR "krf couldn't create /proc/" KRF_PROC_DIR);
    return -2;
  }

  if (proc_create(KRF_RNG_STATE_FILENAME, 644, krf_dir, &rng_state_file_ops) == NULL ||
      proc_create(KRF_PERSONALITY_FILENAME, 644, krf_dir, &personality_file_ops) == NULL ||
      proc_create(KRF_PROBABILITY_FILENAME, 644, krf_dir, &probability_file_ops) == NULL ||
      proc_create(KRF_CONTROL_FILENAME, 644, krf_dir, &control_file_ops) == NULL ||
      proc_create(KRF_LOG_FAULTS_FILENAME, 644, krf_dir, &log_faults_file_ops) == NULL ||
      proc_create(KRF_TARGETING_FILENAME, 644, krf_dir, &targeting_file_ops) == NULL) {
    printk(KERN_ERR "krf couldn't create /proc entries\n");
    return -3;
  }

  return 0;
}

/*static void krf_flush_table(void) {
  int nr;

  for (nr = 0; nr < KRF_NR_SYSCALLS; nr++) {
    if (krf_sys_call_table[nr]) {
      KRF_CR0_WRITE_UNLOCK({ sys_call_table[nr] = krf_sys_call_table[nr]; });
    }
  }
}
*/

static void krf_teardown(void) {
  krf_flush_table();
  remove_proc_subtree(KRF_PROC_DIR, NULL);
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

static ssize_t personality_file_read(struct file *f, char __user *ubuf, size_t size, loff_t *off) {
  char buf[KRF_PROCFS_MAX_SIZE + 1] = {0};
  size_t buflen = 0;

  sprintf(buf, "%u\n", krf_personality);
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

static ssize_t personality_file_write(struct file *f, const char __user *ubuf, size_t size,
                                      loff_t *off) {
  char buf[KRF_PROCFS_MAX_SIZE + 1] = {0};
  size_t buflen = 0;

  if (size > KRF_PROCFS_MAX_SIZE) {
    size = KRF_PROCFS_MAX_SIZE;
  }

  if (copy_from_user(buf, ubuf, size)) {
    return -EFAULT;
  }

  if (kstrtouint(buf, 0, &krf_personality) < 0) {
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
