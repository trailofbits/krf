#include <sys/types.h>
#include <sys/module.h>
#include <sys/systm.h>  /* uprintf */
#include <sys/errno.h>
#include <sys/param.h>  /* defines used in kernel.h */
#include <sys/kernel.h> /* types used in module initialization */

static int krf_loader(struct module *m, int what, void *arg) {
  int err = 0;
  switch (what) {
  case MOD_LOAD:
    uprintf("krf loaded\n");
    break;
  case MOD_UNLOAD:
    uprintf("krf unloaded\n");
    break;
  default:
    err = EOPNOTSUPP;
    break;
  }
  return (err);
}

static moduledata_t krf_mod = {
  "krf",
  krf_loader,
  NULL
};

DECLARE_MODULE(krf, krf_mod, SI_SUB_KLD, SI_ORDER_ANY);
