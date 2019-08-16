#pragma once
#include "../common/common.h"

/* All of our options are unsigned ints,
 * so 32 bytes should be more than enough for their string reps
 * plus a trailing newline.
 */
#define KRF_PROCFS_MAX_SIZE 255

extern unsigned int krf_rng_state;
extern unsigned int krf_probability;
extern unsigned int krf_log_faults;
extern unsigned int krf_targeting;

#define KRF_T_MODE_MAX 31
#define KRF_T_MODE_MAX_MASK (1 << KRF_T_MODE_MAX)

_Static_assert(((KRF_T_NUM_MODES) <= (KRF_T_MODE_MAX)), "Too many modes");

typedef struct {
  unsigned int mode_mask;
  unsigned int target_data[KRF_T_MODE_MAX];
} krf_target_options_t;

extern krf_target_options_t krf_target_options;
