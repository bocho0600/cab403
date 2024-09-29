#include <stdio.h>
#include <stdarg.h>

// Variadic function to calculate the sum of integers
int sum(int count, ...) {
    va_list args; // va_list type to store the arguments
    int total = 0;
    
    // Initialize va_list
    va_start(args, count);
    
    // Retrieve and sum the arguments
    for (int i = 0; i < count; i++) {
        total += va_arg(args, int);
    }
    
    // Clean up va_list
    va_end(args);
    
    return total;
}

int main() {
    // Call the variadic function with different numbers of arguments
    printf("Sum of 3, 4, 5: %d\n", sum(3, 3, 4, 5));    // Output: 12
    printf("Sum of 1, 2, 3, 4, 5: %d\n", sum(5, 1, 2, 3, 4, 5)); // Output: 15
    
    return 0;
}
