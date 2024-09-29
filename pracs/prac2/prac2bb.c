#include <stdio.h>
#define PI 3.14159

int main(int argc, char *argv[]) {
	float userInput = 0;
	for (int i =0; i < 5; i++) {
		printf("\nEnter the radius of circle number %d- ", i+1);
		scanf("%f", &userInput); // Get the radius of the circle
        printf("The area of the circle is %.2f\n", PI * userInput * userInput);
		// Output the area of the circle - 1limit to 2 decimal places
	}
	return 0;
}
