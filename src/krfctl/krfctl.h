#pragma once

typedef struct syscall_lookup_t {
  const char *sys_name;
  /* no point in storing it as an int if we're just going to convert it */
  const char *sys_num;
} syscall_lookup_t;

extern syscall_lookup_t syscall_lookup_table[];
