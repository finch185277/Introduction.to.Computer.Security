#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main() {
  int random;
  int value;

  // We need a random value to prevent someone break us!
  random = rand();

  // Get value from stdin
  scanf("%d", &value);

  // BIG DEAL! DON'T LET OTHER BREAK IT!
  if ((value ^ random) == 0x0B16DEA1) {
    printf("Good!\n");

    setreuid(0, 0);
    system("/bin/cat flag");
    return 0;
  }

  printf("You really need to read the manual, otherwise you you should try "
         "2^32 cases.\n");
  return 0;
}
/*
Name: you_should_read_manual
solution:

rand() is fake random number!
write a program like that:
```
#include <stdio.h>
#include <stdlib.h>

int main(){
        int random, key;
        random = rand();
        key = 0x0B16DEA1 ^ random;
        printf("%d\n", key);
}
```
$ g++ -g -o get_key get_key.cpp
$ ./get_key
1620941766
$ ./you_should_read_manual
1620941766
Good!
REFERENCES_now_you_KnoW_HOW_IMPORTANT_IS_Documentation_DO_YOU?
*/
