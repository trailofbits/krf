#pragma once
/* Common defines and types needed across the krf utils and module */

/* Strings used to generate procfs filenames and sysctl strings */
#define KRF_PROC_DIR "krf"
#define KRF_RNG_STATE_FILENAME "rng_state"
#define KRF_PROBABILITY_FILENAME "probability"
#define KRF_CONTROL_FILENAME "control"
#define KRF_LOG_FAULTS_FILENAME "log_faults"
#define KRF_TARGETING_FILENAME "targeting"

/* Targeting modes */
typedef enum {
  KRF_T_MODE_PERSONALITY = 0,
  KRF_T_MODE_PID,
  KRF_T_MODE_UID,
  KRF_T_MODE_GID,
  KRF_T_MODE_INODE,
  // Insert new modes here
  KRF_T_NUM_MODES
} krf_target_mode_t;

/* Netlink Defines */
/* Protocol family, consistent in both kernel prog and user prog. */
#define NETLINK_KRF 28
/* Multicast group, consistent in both kernel prog and user prog. */
#define NETLINK_MYGROUP 28
