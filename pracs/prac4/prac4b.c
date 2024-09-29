#include <stdio.h>

// Function to be pointed to
void greet(void) {
    printf("Hello, world!\n");
}

int main() {
    // Declare a function pointer and assign it to the function `greet`
    void (*func_ptr)(void) = greet;
    
    // Call the function using the function pointer
    func_ptr();
    
    return 0;
}
