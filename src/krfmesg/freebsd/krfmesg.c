#include <stdio.h>
#include <err.h>

int platform_main(int argc, char *argv[]) {
  errx(1, "krfmesg not implemented on FreeBSD, since no netlink sockets");
  return 0;
}
