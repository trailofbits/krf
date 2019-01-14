#pragma once

typedef struct syscall_lookup_t {
  const char *sys_name;
  /* no point in storing it as an int if we're just going to convert it */
  const char *sys_num;
} syscall_lookup_t;

typedef struct fault_profile_t {
  const char *profile;
  /* GCC doesn't like flexible array initialization within
   * structures, so just give ourselves enough room for
   * sensibly sized profiles.
   */
  const char *syscalls[128];
} fault_profile_t;

extern syscall_lookup_t syscall_lookup_table[];
extern fault_profile_t fault_profile_table[];
