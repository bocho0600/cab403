#include <stdio.h>
#include <stdlib.h>

#define BUFFER_SIZE 1024

void compare_floats(float a, float b) {
    if (a == b) {
        printf("Equal\n");  // Warning due to -Wfloat-equal
    }
}

void check_macro() {
    #ifdef UNDEFINED_MACRO
        printf("Macro is defined.\n");
    #endif  // Warning due to -Wundef
}
int hello = 0;
int main() {
    // Warnings due to -Wwrite-strings
    char *str1 = "This is a string";  // Warning: assigning string literal to a non-const pointer
    const char *str2 = "This is a string";
      //test \ ->this is line spicing
    float x = 1.0f, y = 1.0f;
    compare_floats(x, y);

    // Warning due to -Wcast-align
    int i = 0;
    char *ptr = (char *)&i;  // Casting integer pointer to char pointer may lead to misalignment

    check_macro();
    return 0;
}
