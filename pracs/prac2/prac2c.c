#include <stdlib.h>

int main()
{
    char *x = malloc(10);
    x[10] = 'a';
    //free(x);
    //use valgrind --leak-check=yes ./prac2d in terminal (cmd) to check for memory leaks
    return 0;
}