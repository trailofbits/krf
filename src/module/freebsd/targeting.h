#pragma once
#include "freebsd.h"
#include "../targeting.h"
#include <sys/proc.h>
#include <sys/lock.h>
#include <sys/mutex.h>
#include <sys/param.h>
#include <sys/vnode.h>
#include <sys/file.h>
#include <sys/filedesc.h>

static __always_inline bool krf_personality(unsigned int target, krf_ctx_t *context) {
  return (context->td_proc->p_flag2 & (target));
}

#ifdef KRF_FREEBSD_UNSAFE_PID_TRAVERSAL
static __always_inline bool krf_pid(unsigned int target, krf_ctx_t *context) {
  struct proc *par = context->td_proc;
  do {
    if (par->p_pid == (target)) {
      return true;
      break;
    }
  } while ((par = par->p_pptr));
  return false;
}
#else // Default: do a check with depth=1 using locks
static __always_inline bool krf_pid(unsigned int target, krf_ctx_t *context) {
  int ret = 0;
  PROC_LOCK(context->td_proc);
  if (context->td_proc->p_pid == (target)) {
    ret = 1;
  } else {
    PROC_LOCK(context->td_proc->p_pptr);
    if (context->td_proc->p_pptr->p_pid == (target))
      ret = 1;
    PROC_UNLOCK(context->td_proc->p_pptr);
  }
  PROC_UNLOCK(context->td_proc);
  return ret;
}
#endif

static __always_inline bool krf_uid(unsigned int target, krf_ctx_t *context) {
  return (context->td_proc->p_ucred->cr_ruid ==
          (target)); // Currently using real UID but could use effective UID (cr_uid)
}

static __always_inline bool krf_gid(unsigned int target, krf_ctx_t *context) {
  return (context->td_proc->p_ucred->cr_rgid == (target));
}

static __always_inline bool krf_inode(unsigned int target, krf_ctx_t *context) {
  int i = 0;
  bool ret = false;
  struct vattr vap;
  struct filedesc *fdp;

  PROC_LOCK(context->td_proc);
  fdp = context->td_proc->p_fd;
  PROC_UNLOCK(context->td_proc);

  if (fdp == NULL)
    return false;

  FILEDESC_SLOCK(context->td_proc->p_fd);
  for (; i <= fdp->fd_lastfile; i++) {
    if (fdp->fd_refcnt <= 0)
      break;
    if (fdp->fd_ofiles[i].fde_file == NULL)
      break;
    if (fdp->fd_ofiles[i].fde_file->f_type != DTYPE_VNODE)
      continue;
    if (fdp->fd_ofiles[i].fde_file->f_vnode == NULL)
      break;

    VI_LOCK(fdp->fd_ofiles[i].fde_file->f_vnode);
    vget(fdp->fd_files->fdt_ofiles[i].fde_file->f_vnode, LK_EXCLUSIVE | LK_INTERLOCK, context);
    if (VOP_GETATTR(fdp->fd_files->fdt_ofiles[i].fde_file->f_vnode, &vap,
                    fdp->fd_files->fdt_ofiles[i].fde_file->f_cred) != 0) {
      vput(fdp->fd_files->fdt_ofiles[i].fde_file->f_vnode);
      break;
    }
    vput(fdp->fd_files->fdt_ofiles[i].fde_file->f_vnode);
    if (target == vap.va_fileid) {
      ret = true;
      break;
    }
  }
  FILEDESC_SUNLOCK(fdp);
  return ret;
}
