#include <linux/netlink.h>
#include <net/netlink.h>
#include <net/net_namespace.h>

#include "netlink.h"

static struct sock *krf_socket;

int krf_netlink_broadcast(char *buf, unsigned message_size) {
  struct sk_buff *skb;
  struct nlmsghdr *nlh;
  int result;
  skb = nlmsg_new(NLMSG_ALIGN(message_size), GFP_KERNEL);
  if (!skb) {
    printk(KERN_ERR "krf: Failed to allocate a new skb\n");
    return -1;
  }
  nlh = nlmsg_put(skb, 0, 1, NLMSG_DONE, message_size, 0);
  strncpy(nlmsg_data(nlh), buf, message_size);
  result = nlmsg_multicast(krf_socket, skb, 0, NETLINK_MYGROUP, GFP_KERNEL);
  if (result < 0) {
    printk(KERN_ERR "krf: Failed to multicast message with error code %d\n", result);
  }
  return result;
}

int setup_netlink_socket(void) {
  struct netlink_kernel_cfg config = {
      .groups = NETLINK_MYGROUP,
  };
  krf_socket = netlink_kernel_create(&init_net, NETLINK_KRF, &config);
  if (krf_socket < 0) {
    printk(KERN_ERR "krf: couldn't create a netlink");
    return -1;
  }
  return 0;
}

void destroy_netlink_socket(void) {
  netlink_kernel_release(krf_socket);
}
