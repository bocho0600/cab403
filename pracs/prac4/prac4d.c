#include <stdio.h>
#include <string.h>

void print_info();

// create struct with person1 variable
struct Person {
	char name[50];
	int citNo;
	float salary;
} person1; // This is a variable called person1

int main(int argc, char *argv[]) {
	// assign value to name of person1
	strcpy(person1.name, "George Orwell"); // Cannot use persin1.name = "George Orwell" after ARRAY has been declared in the struct
	// assign values to other person1 variables
	person1.citNo = 1984;
	person1. salary = 2500;
	// print struct variables
      print_info();
      // assign values to person1 variables
      char name[50];
      printf("Enter the persons first name - "); scanf("%s", name);
      strcpy(person1.name, name);
      printf("Enter the persons citizenship number - "); scanf("%d", &person1.citNo);
      printf("Enter the persons salary - "); scanf("%f", &person1.salary);
      printf("New Person Information Output: \n");
      print_info();
	return 0;
}

void print_info(){
      printf("\nName: %s\n", person1.name);
	printf("\nCitizenship No.: %d\n", person1.citNo);
	printf("\nSalary: %.2f\n", person1.salary);
	printf("\n\n");
}