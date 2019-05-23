#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define MAX_LEN 100

char buf[MAX_LEN];

int main(int argc, char *argv[]) {
  int fd;
  int len;

  if (argc < 2) {
    printf("usage: fildes [INT NUMBER]\n");
    return 0;
  }

  fd = atoi(argv[1]) - 0xCAFE;
  len = read(fd, buf, MAX_LEN);

  if (!strncmp("YOUSHALLNOTPASS\n", buf, MAX_LEN)) {
    printf("Maybe you learn something :)\n");

    setreuid(0, 0);
    system("/bin/cat flag");
    exit(0);
  }

  printf("Go learn about Linux file IO, and file descriptor\n");
  return 0;
}
/*
solution:

$ python3 -c "print(0xCAFE)"
51966
$ ./fildes 51966
YOUSHALLNOTPASS
Maybe you learn something :)
Sure_This_1s_th3_first_FlAG_you_have_HOPE_you_learn_fd_^_^

ref: https://0xrick.github.io/pwn/fd/
*/
