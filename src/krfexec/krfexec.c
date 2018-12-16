#include <stdio.h>
#include <string.h>
#include <sys/personality.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/fcntl.h>

#define PERSONALITY_FILE "/proc/krf/personality"

int main(int argc, char *argv[]) {
  int fd;
  char pers_str[33] = {0};
  unsigned int pers;

  if ((fd = open(PERSONALITY_FILE, O_RDONLY)) < 0) {
    perror("open " PERSONALITY_FILE);
    return errno;
  }

  if (read(fd, pers_str, sizeof(pers_str) - 1) < 0) {
    perror("read " PERSONALITY_FILE);
    return errno;
  }

  if (sscanf(pers_str, "%u", &pers) != 1) {
    fprintf(stderr, "oddity: personality isn't numeric?\n");
    return 1;
  }

  if (personality(pers) < 0) {
    perror("personality");
    return errno;
  }

  if (argc < 2 || !strcmp(argv[1], "-h")) {
    printf("usage: krfexec <command or file> [args]\n");
    return 1;
  }

  if (execvp(argv[1], argv + 1) < 0) {
    perror("exec");
    return errno;
  }

  return 0; /* noreturn */
}
