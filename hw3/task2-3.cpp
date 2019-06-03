/*
 * Random Password Authenticate
 *
 * usage: rpa [passcode]
 *
 * This is a Random Password Authenticate (RPA) skeleton,
 * I wish there have no mistakes in the code.
 *
 * Can you help me find out is there any bug?
 * Sure, I'll give you the reward.
 *
 * HINT: DO NOT BRUTEFORCE
 */

#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define MAX_LEN 200
#define RPA_LEN 2

typedef unsigned long long int UInt;

int main(int argc, char *argv[]) {
  char tmp_fname[MAX_LEN];
  UInt rpa[RPA_LEN];
  UInt passcode;
  int fd;

  if (argc != 2) {
    printf("usage: rpa [passcode]\n");
    exit(-1);
  }

  // Generate Random Password
  if ((fd = open("/dev/urandom", O_RDONLY)) == -1) {
    perror("Error: ");
    exit(-1);
  }

  if (read(fd, rpa, RPA_LEN * sizeof(UInt)) != (RPA_LEN * sizeof(UInt))) {
    printf("Wierd...\n");
    exit(-1);
  }
  close(fd);

  sprintf(tmp_fname, "/tmp/%llu", rpa[0]);
  FILE *fp = fopen(tmp_fname, "w");
  if (!fp) {
    printf("Cannot open file\n");
    exit(-1);
  }

  fwrite(&rpa[1], sizeof(UInt), 1, fp);
  fclose(fp);

  printf("Random Password Generated!\n");

  // Load Random Password
  fp = fopen(tmp_fname, "r");
  if (!fp) {
    printf("Cannot open file\n");
    exit(-1);
  }

  fread(&passcode, sizeof(UInt), 1, fp);
  fclose(fp);

  // Compare the RP with input passcode
  if (strtoul(argv[1], NULL, 16) == passcode) {
    printf("You did it!\n");

    setreuid(0, 0);
    system("/bin/cat flag");
  } else {
    printf("RPA mismatch\n");
  }

  unlink(tmp_fname);
  return 0;
}
/*
$ ulimit -f 0 && python3 -c "import os; os.system('./rpa 0')"
*/
