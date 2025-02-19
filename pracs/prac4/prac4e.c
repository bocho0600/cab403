#include <stdio.h>
#include <string.h>

// struct with typedef person
typedef struct Person {
	char name[50];
	int citNo;
	float salary;
} person; // This is a datatype, not a variable

int main() {
	// create  Person variable, We use the typedef keyword to create an alias name for data types
	person p1;
	// assign value to name of p1
	strcpy(p1.name, "George Orwell");

	// assign values to other p1 variables
	p1.citNo = 1984;
	p1. salary = 2500;

	// print struct variables
	printf("\n\nName: %s\n", p1.name);
	printf("\nCitizenship No.: %d\n", p1.citNo);
	printf("\nSalary: %.2f", p1.salary);
	printf("\n\n");
	return 0;
}
