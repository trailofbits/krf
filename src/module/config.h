#pragma once

#define KRF_PROC_DIR "krf"
#define KRF_RNG_STATE_FILENAME "rng_state"
#define KRF_PROBABILITY_FILENAME "probability"
#define KRF_CONTROL_FILENAME "control"
#define KRF_LOG_FAULTS_FILENAME "log_faults"
#define KRF_TARGETING_FILENAME "targeting"

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

typedef enum {
  KRF_T_MODE_PERSONALITY = 0,
  KRF_T_MODE_PID,
  KRF_T_MODE_UID,
  KRF_T_MODE_GID,
  KRF_T_MODE_FILE,
  // Insert new modes here
  KRF_T_NUM_MODES
} krf_target_mode_t;

_Static_assert(((KRF_T_NUM_MODES) <= (KRF_T_MODE_MAX)), "Too many modes");

typedef struct {
  unsigned int mode_mask;
  unsigned int target_data[KRF_T_MODE_MAX];
} krf_target_options_t;

extern krf_target_options_t krf_target_options;
