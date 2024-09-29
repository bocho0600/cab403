#include <stdio.h>
#define arrayLength(array) (sizeof((array))/sizeof((array)[0])) // Function like macro

int main(int argc, char *argv[]) {
 int arrayOne[] = {12, 15, 2, 19, 43, 22};
 int length = sizeof(arrayOne)/sizeof(arrayOne[0]); // (total length of the array) / (length of 1 element) = number of elements
 for (int i=0; i < length; i++) {
     printf("arrayOne[%d] = %d  ||  ", i, arrayOne[i]);
 } 
 printf("\n\nThe length of the array is %d\n", length);
 printf("The length of the array using macros is %ld\n", arrayLength(arrayOne));
 return 0;
}
