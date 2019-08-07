#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <linux/netlink.h>
#include <unistd.h>

/* Protocol family, consistent in both kernel prog and user prog. */
#define NETLINK_KRF 28
/* Multicast group, consistent in both kernel prog and user prog. */
#define KRF_MGRP 28

int open_netlink(void) {
  int sock;
  struct sockaddr_nl addr;
  int group = KRF_MGRP;

  sock = socket(AF_NETLINK, SOCK_RAW, NETLINK_KRF);
  if (sock < 0) {
    printf("Failed to make socket. Is krf module installed?\n");
    return sock;
  }

  memset((void *)&addr, 0, sizeof(addr));
  addr.nl_family = AF_NETLINK;
  addr.nl_pid = getpid();
  /* This doesn't work for some reason. See the setsockopt() below. */
  /* addr.nl_groups = KRF_MGRP; */

  if (bind(sock, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
    printf("Failed to bind socket\n");
    return -1;
  }

  /*
   * 270 is SOL_NETLINK. See
   * http://lxr.free-electrons.com/source/include/linux/socket.h?v=4.1#L314
   * and
   * http://stackoverflow.com/questions/17732044/
   */
  if (setsockopt(sock, 270, NETLINK_ADD_MEMBERSHIP, &group, sizeof(group)) < 0) {
    printf("Failed to setsockopt. Is krfmesg being run with sudo?\n");
    // Will need to be run with sudo
    return -1;
  }

  return sock;
}

void read_event(int sock) {
  struct sockaddr_nl nladdr;
  struct msghdr msg;
  struct iovec iov;
  char buffer[65536];
  int ret;

  iov.iov_base = (void *)buffer;
  iov.iov_len = sizeof(buffer);
  msg.msg_name = (void *)&(nladdr);
  msg.msg_namelen = sizeof(nladdr);
  msg.msg_iov = &iov;
  msg.msg_iovlen = 1;

  ret = recvmsg(sock, &msg, 0);
  if (ret < 0)
    printf("ret < 0.\n");
  else
    printf("%s", (char *)NLMSG_DATA((struct nlmsghdr *)&buffer));
}

int platformMain(int argc, char *argv[]) {
  int nls;

  nls = open_netlink();
  if (nls < 0)
    return nls;

  while (1)
    read_event(nls);

  return 0;
}
