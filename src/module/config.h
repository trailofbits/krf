#pragma once

#define KRF_PROC_DIR "krf"
#define KRF_RNG_STATE_FILENAME "rng_state"
#define KRF_PERSONALITY_FILENAME "personality" /* TODO(ww) */
#define KRF_PROBABILITY_FILENAME "probability" /* TODO(ww) */
#define KRF_TARGETED_UID_FILENAME "targeted_uid"

/* All of our options are unsigned ints,
 * so 32 bytes should be more than enough for their string reps
 * plus a trailing newline.
 */
#define KRF_PROCFS_MAX_SIZE 32

extern unsigned int krf_rng_state;
extern unsigned int krf_personality;
extern unsigned int krf_probability;
extern unsigned int krf_targeted_uid;
