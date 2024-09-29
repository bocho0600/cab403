#include <stdio.h>
#include <string.h>

int main(int argc, char *argv[]) {
    char *string;
    printf("Enter String: ");
    scanf("%[^\n]", string); // This will read the string until a newline (entered) is encountered
    printf("String output: %s\n", string);
 return 0;
}