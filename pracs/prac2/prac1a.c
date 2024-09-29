#include <stdio.h>

int main(int ag, char *argv[]) {
      printf("The number of command line arguments is %d\n", ag);
      printf("The first argument is %s\n", argv[0]);
      printf("The second argument is %s\n", argv[1]);
      printf("The third argument is %s\n", argv[2]);
      printf("The fourth argument is %s\n", argv[3]); 
    return 0;

}