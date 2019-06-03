#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define LUCKY_POT_LENGTH 6

void play() {
  int i, j;
  int fd;
  int match = 0;
  unsigned char lucky[LUCKY_POT_LENGTH] = {0};
  unsigned char submit[LUCKY_POT_LENGTH] = {0};

  printf("Buy yourself a luck, submit you %d lucky bytes: ", LUCKY_POT_LENGTH);
  fflush(stdout);

  // Get submit
  read(STDIN_FILENO, submit, LUCKY_POT_LENGTH);

  printf("Lucky ball, go!\n");

  // Generate lucky numbers
  fd = open("/dev/urandom", O_RDONLY);
  if (!~fd) {
    perror("Error: ");
    exit(-1);
  }

  if (read(fd, lucky, LUCKY_POT_LENGTH) != LUCKY_POT_LENGTH) {
    printf("This should not happend\n");
    exit(-1);
  }

  for (i = 0; i < LUCKY_POT_LENGTH; ++i) {
    lucky[i] = (lucky[i] % 49) + 1; // 1 ~ 49
  }
  close(fd);

  // Publis the lucky number
  printf("The lucky number: ");
  for (i = 0; i < LUCKY_POT_LENGTH; ++i) {
    printf("%d ", lucky[i]);
  }
  printf("\n");

  // Do we match the lucky pot?
  for (i = 0; i < LUCKY_POT_LENGTH; ++i) {
    for (j = 0; j < LUCKY_POT_LENGTH; ++j) {
      if (lucky[i] == submit[j]) {
        match++;
      }
    }
  }

  if (match == LUCKY_POT_LENGTH) {
    setreuid(0, 0);
    system("/bin/cat flag");
  } else {
    printf("Bad luck...\n");
  }
}

void help() {
  printf(" - Lucky Pot Rules - \n");
  printf(
      "Lucky Pot is a lotto game that you must submit 6 number from 1 ~ 49.\n");
  printf("You should match 6 number you submit to lucky pot number in "
         "regardless of the order.\n");
  printf("If you matched, you will get your *reward*\n");
  printf("Good luck, my friend.\n");
  printf("\n");
}

int main() {
  int v;

  while (1) {
    printf("Select menu: \n");
    printf("  1. Play Lucky Pot\n");
    printf("  2. Help\n");
    printf("  3. Exit\n");

    scanf("%d", &v);
    switch (v) {
    case 1:
      play();
      break;
    case 2:
      help();
      break;
    case 3:
      printf("Seeya\n");
      exit(0);
    default:
      printf("Invalid menu number\n");
      break;
    }
  }
}
