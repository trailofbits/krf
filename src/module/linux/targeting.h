#pragma once
#include "linux.h"
#include "../targeting.h"
#include <linux/fs.h>
#include <linux/fdtable.h>

static __always_inline bool krf_personality(unsigned int target, krf_ctx_t *context) {
  return (context->personality & (target));
}
static __always_inline bool krf_pid(unsigned int target, krf_ctx_t *context) {
  return (context->pid == (target));
}
static __always_inline bool krf_uid(unsigned int target, krf_ctx_t *context) {
  return (context->cred->uid.val == (target));
}
static __always_inline bool krf_gid(unsigned int target, krf_ctx_t *context) {
  return (context->cred->gid.val == (target));
}
static __always_inline bool krf_inode(unsigned int target, krf_ctx_t *context) {
  int i = 0;
  while (context->files->fdt->fd[i] != NULL) {
    if ((target == context->files->fdt->fd[i]->f_inode->i_ino)) {
      return true;
    }
    i++;
  }
  return false;
}
