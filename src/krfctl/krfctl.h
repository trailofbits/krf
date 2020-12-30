#pragma once

typedef struct syscall_lookup_t {
  const char *sys_name;
  /* no point in storing it as an int if we're just going to convert it */
  const char *sys_num;
} syscall_lookup_t;

typedef struct fault_profile_t {
  const char *profile;
  const char *description;
  /* GCC doesn't like flexible array initialization within
   * structures, so just give ourselves enough room for
   * sensibly sized profiles.
   */
  const char *syscalls[256];
} fault_profile_t;

extern syscall_lookup_t syscall_lookup_table[];
extern fault_profile_t fault_profile_table[];

const char *lookup_syscall_number(const char *sys_name);
int fault_syscall(const char *sys_name);
void clear_faulty_calls(void);
void set_rng_state(const char *state);
void set_prob_state(const char *state);
void toggle_fault_logging(void);
void set_targeting(unsigned int mode, const char *data);
