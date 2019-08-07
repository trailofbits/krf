#include <stdio.h>

int platformMain(int argc, char *argv[]) {
  printf("krfmesg not implemented on FreeBSD, since no netlink sockets\n");
  return 0;
}
