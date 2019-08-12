#include <err.h>
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
    err(1, "Failed to make socket. Is krf module installed?");
    return sock;
  }

  memset((void *)&addr, 0, sizeof(addr));
  addr.nl_family = AF_NETLINK;
  addr.nl_pid = getpid();
  /* This doesn't work for some reason. See the setsockopt() below. */
  /* addr.nl_groups = KRF_MGRP; */

  if (bind(sock, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
    err(1, "Failed to bind socket");
    return -1;
  }

  /*
   * 270 is SOL_NETLINK. See
   * http://lxr.free-electrons.com/source/include/linux/socket.h?v=4.1#L314
   * and
   * http://stackoverflow.com/questions/17732044/
   */
  if (setsockopt(sock, 270, NETLINK_ADD_MEMBERSHIP, &group, sizeof(group)) < 0) {
    err(1, "Failed to setsockopt. Is krfmesg being run with sudo?");
    // Will need to be run with sudo
    return -1;
  }

  return sock;
}

void read_event(int sock) {
  struct sockaddr_nl nladdr;
  char buffer[65536];
  int ret;
  struct iovec iov = {
      .iov_base = (void *)buffer,
      .iov_len = sizeof(buffer),
  };
  struct msghdr msg = {
      .msg_name = (void *)&(nladdr),
      .msg_namelen = sizeof(nladdr),
      .msg_iov = &iov,
      .msg_iovlen = 1,
  };

  ret = recvmsg(sock, &msg, 0);
  if (ret < 0) {
    err(1, "ret < 0.");
  } else {
    printf("%s", (char *)NLMSG_DATA((struct nlmsghdr *)&buffer));
  }
}

int platform_main(int argc, char *argv[]) {
  int nls;

  nls = open_netlink();
  if (nls < 0)
    return nls;

  while (1)
    read_event(nls);

  return 0;
}
