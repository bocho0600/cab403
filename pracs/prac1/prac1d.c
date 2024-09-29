#include <stdio.h>

int main(void) {
    int a = 45;
    int b = 22;  
    int *p1 = 0;
    int *p2 = 0;
    /* & symbol tells p1 and p2 to point to the memory address where a and b is stored respectively */
    p1 = &a; // -> p1 = address of a
    p2 = &b; // -> p2 = address of b
    // *p1 = 0x7fffbf7b3bfc 
    // *p2 = 0x7fffbf7b3bf8
    /* 
    To print the value in the memory location that pointer is pointing to we need to use the dereferencing operator * 
    */

    printf("The value of p1 is %d and the memory address location is %p\n", *p1, p1);
    printf("The value of p2 is %d and the memory address location is %p\n", *p2, p2);
    printf("The value of a is %d and the memory address location is %p\n", a, &a);
    return 0;

}