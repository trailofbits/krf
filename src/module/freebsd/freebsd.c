#include "freebsd.h"
#include "../targeting.h"
/*#include <sys/types.h>
#include <sys/systm.h>
#include <sys/sysproto.h>*/
#include <sys/proc.h>
#include <sys/lock.h>
#include <sys/mutex.h>

bool krf_personality(unsigned int target, krf_ctx_t *context) {
  return (context->td_proc->p_flag2 & (target));
}

#ifdef KRF_FREEBSD_UNSAFE_PID_TRAVERSAL
bool krf_pid(unsigned int target, krf_ctx_t *context) {
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
bool krf_pid(unsigned int target, krf_ctx_t *context) {
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

bool krf_uid(unsigned int target, krf_ctx_t *context) {
  return (context->td_proc->p_ucred->cr_ruid ==
          (target)); // Currently using real UID but could use effective UID (cr_uid)
}

bool krf_gid(unsigned int target, krf_ctx_t *context) {
  return (context->td_proc->p_ucred->cr_rgid == (target));
}

bool krf_file(unsigned int target, krf_ctx_t *context) {
  return false;
}
