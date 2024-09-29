#include <stdio.h>

// Function to be pointed to
int add(int a, int b) {
    return a + b;
}

// Another function that takes a function pointer as an argument
int compute(int (*operation)(int, int), int x, int y) {
    return operation(x, y);
}

int main() {
    // Call compute with the add function pointer
    int result = compute(add, 5, 3);
    printf("Result: %d\n", result); // Output: Result: 8
    
    return 0;
}
