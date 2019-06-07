#pragma once

#define KRF_PROC_DIR "krf"
#define KRF_RNG_STATE_FILENAME "rng_state"
#define KRF_PERSONALITY_FILENAME "personality"
#define KRF_PROBABILITY_FILENAME "probability"
#define KRF_CONTROL_FILENAME "control"
#define KRF_LOG_FAULTS_FILENAME "log_faults"
#define KRF_PID_TARGET_FILENAME "pid_target"
#define KRF_UID_TARGET_FILENAME "uid_target"
#define KRF_GID_TARGET_FILENAME "gid_target"

/* All of our options are unsigned ints,
 * so 32 bytes should be more than enough for their string reps
 * plus a trailing newline.
 */
#define KRF_PROCFS_MAX_SIZE 32

extern unsigned int krf_rng_state;
extern unsigned int krf_personality;
extern unsigned int krf_probability;
extern unsigned int krf_log_faults;
extern unsigned int krf_pid_target;
extern unsigned int krf_uid_target;
extern unsigned int krf_gid_target;

// The targeting options are stored in this variable
extern unsigned char krf_targeting_options;

#define PID_MASK (1 << 1)
#define UID_MASK (1 << 2)
#define GID_MASK (1 << 3)

#define KRF_PID_TARGETING() ((krf_targeting_options) & (PID_MASK))
#define KRF_UID_TARGETING() ((krf_targeting_options) & (UID_MASK))
#define KRF_GID_TARGETING() ((krf_targeting_options) & (GID_MASK))

#define KRF_ENABLE_PID() ((krf_targeting_options) |= (PID_MASK))
#define KRF_DISABLE_PID() ((krf_targeting_options) &= !(PID_MASK))
#define KRF_ENABLE_UID() ((krf_targeting_options) |= (UID_MASK))
#define KRF_DISABLE_UID() ((krf_targeting_options) &= !(UID_MASK))
#define KRF_ENABLE_GID() ((krf_targeting_options) |= (GID_MASK))
#define KRF_DISABLE_GID() ((krf_targeting_options) &= !(GID_MASK))
