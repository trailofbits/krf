#pragma once

#define NETLINK_KRF 28
#define NETLINK_MYGROUP 28
#define KRF_NETLINK_BUF_SIZE 256 // Arbitrary maximum message size

int krf_netlink_broadcast(char *buf, unsigned message_size);
int setup_netlink_socket(void);
void destroy_netlink_socket(void);
