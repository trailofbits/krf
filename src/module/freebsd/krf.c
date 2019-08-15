#include <sys/types.h>
#include <sys/param.h>
#include <sys/proc.h>
#include <sys/module.h>
#include <sys/sysproto.h>
#include <sys/sysent.h>
#include <sys/kernel.h>
#include <sys/systm.h>
#include <sys/sysctl.h>
#include <sys/conf.h>

#include <sys/syscall.h>

#include "../config.h"
#include "../krf.h"
#include "syscalls.h"

static struct sysctl_ctx_list clist;
static struct sysctl_oid *krf_sysctl_root;
static unsigned int krf_control;
static char krf_targetings[13];

static int control_file_sysctl(SYSCTL_HANDLER_ARGS) {
  int syscall = -1;
  int err = 0;

  if (sysctl_handle_int(oidp, &syscall, 0, req)) {
    return -1;
  } else if (req->newptr) {
    err = control_file_handler(syscall);
    if (err < 0)
      return -err;
  } else {
    // read request?
  }
  return err;
}

static int targeting_file_sysctl(SYSCTL_HANDLER_ARGS) {
  int err = 0;
  krf_target_mode_t mode;
  unsigned int data;

  err = sysctl_handle_string(oidp, &krf_targetings, 13, req);
  if (err) {
    return -err;
  } else if (req->newptr) {
    if (sscanf(krf_targetings, "%u %u", &mode, &data) != 2) {
      return EINVAL;
    }
    if (targeting_file_write_handler(mode, data) < 0) {
      return EINVAL;
    }
  } else {
    // read request?
  }
  return err;
}

static int krf_init() {
  int err = 0;
  sysctl_ctx_init(&clist);
  if (!(krf_sysctl_root = SYSCTL_ADD_ROOT_NODE(&clist, OID_AUTO, KRF_PROC_DIR, CTLFLAG_RW, 0,
                                               "krf sysctl root node"))) {
    uprintf("krf error: Failed to add root sysctl node.\n");
    return -1;
  }

  memset(krf_faultable_table, 0, KRF_NR_SYSCALLS * sizeof(sy_call_t *));
  for (unsigned int i = 0; i < KRF_NR_SYSCALLS; i++) {
    krf_sys_call_table[i] = sysent[i].sy_call;
  }

  SYSCTL_ADD_UINT(&clist, SYSCTL_CHILDREN(krf_sysctl_root), OID_AUTO, KRF_PROBABILITY_FILENAME,
                  CTLFLAG_ANYBODY | CTLFLAG_RW, &krf_probability, krf_probability,
                  "Reciprocal of the probability of a fault");
  SYSCTL_ADD_UINT(&clist, SYSCTL_CHILDREN(krf_sysctl_root), OID_AUTO, KRF_RNG_STATE_FILENAME,
                  CTLFLAG_ANYBODY | CTLFLAG_RW, &krf_rng_state, krf_rng_state,
                  "Sets the current RNG state");
  SYSCTL_ADD_UINT(&clist, SYSCTL_CHILDREN(krf_sysctl_root), OID_AUTO, KRF_LOG_FAULTS_FILENAME,
                  CTLFLAG_ANYBODY | CTLFLAG_RW, &krf_log_faults, krf_log_faults,
                  "Toggle logging faults to syslog");
  SYSCTL_ADD_PROC(&clist, SYSCTL_CHILDREN(krf_sysctl_root), OID_AUTO, KRF_CONTROL_FILENAME,
                  CTLFLAG_ANYBODY | CTLTYPE_UINT | CTLFLAG_WR, &krf_control, krf_control,
                  control_file_sysctl, "IU", "Enables specific syscall faults");
  SYSCTL_ADD_PROC(&clist, SYSCTL_CHILDREN(krf_sysctl_root), OID_AUTO, KRF_TARGETING_FILENAME,
                  CTLFLAG_ANYBODY | CTLTYPE_STRING | CTLFLAG_WR, &krf_targetings, 13,
                  targeting_file_sysctl, "A", "Enables specific targeting options");
  return err;
}

static int krf_teardown() {
  krf_flush_table();
  sysctl_remove_oid(krf_sysctl_root, 1, 0);
  sysctl_ctx_free(&clist);
  return 0;
}

static int krf_loader(struct module *m, int what, void *arg) {
  int err = 0;
  switch (what) {
  case MOD_LOAD:
    err = krf_init();
    if (err != 0)
      uprintf("krf_init failed with %d\n", err);

#include "krf.gen.x"

    uprintf("krf: loaded\n");
    break;
  case MOD_UNLOAD:
    krf_teardown();
    uprintf("krf: unloaded\n");
    break;
  default:
    err = EOPNOTSUPP;
    break;
  }
  return (err);
}

static moduledata_t krf_mod = {"krf", krf_loader, NULL};

DECLARE_MODULE(krf, krf_mod, SI_SUB_EXEC, SI_ORDER_ANY);
