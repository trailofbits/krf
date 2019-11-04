#include <err.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <linux/netlink.h>
#include <unistd.h>
#include <signal.h>
#include "../../common/common.h"

static sig_atomic_t exiting;

int open_netlink(void) {
  int sock;
  struct sockaddr_nl addr;
  int group = NETLINK_MYGROUP;

  sock = socket(AF_NETLINK, SOCK_RAW, NETLINK_KRF);

  if (sock < 0) {
    if (errno == EPROTONOSUPPORT) {
      errx(1, "NETLINK_KRF protocol not found.\n"
              "Check to ensure that the KRF module (krfx) is loaded.");
    } else {
      err(errno, "socket");
    }
  }

  memset((void *)&addr, 0, sizeof(addr));
  addr.nl_family = AF_NETLINK;
  addr.nl_pid = getpid();
  /* This doesn't work for some reason. See the setsockopt() below. */
  /* addr.nl_groups = NETLINK_MYGROUP; */

  if (bind(sock, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
    err(1, "Failed to bind socket");
  }

  /*
   * 270 is SOL_NETLINK. See
   * http://lxr.free-electrons.com/source/include/linux/socket.h?v=4.1#L314
   * and
   * http://stackoverflow.com/questions/17732044/
   */
  if (setsockopt(sock, 270, NETLINK_ADD_MEMBERSHIP, &group, sizeof(group)) < 0) {
    err(1, "Failed to setsockopt");
    // Will need to be run with sudo
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
    err(1, "recvmsg");
  }
  printf("%s", (char *)NLMSG_DATA((struct nlmsghdr *)&buffer));
}

static void exit_sig(int signo) {
  exiting = 1;
}

int platform_main(int argc, char *argv[]) {
  sigaction(SIGINT, &(struct sigaction){.sa_handler = exit_sig}, NULL);
  sigaction(SIGTERM, &(struct sigaction){.sa_handler = exit_sig}, NULL);
  sigaction(SIGABRT, &(struct sigaction){.sa_handler = exit_sig}, NULL);

  int nls = open_netlink();

  while (!exiting) {
    read_event(nls);
  }

  close(nls);
  return 0;
}
