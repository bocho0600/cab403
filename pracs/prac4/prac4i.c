#include <stdio.h>

int main(void) {
    
	struct student {
	char id[15];
	char firstname[64];
	char lastname[64];
	float points;
}; 
 
	struct student studentDetails;
	struct student  *studentPointer= NULL; // This is a struct pointer
	studentPointer = &studentDetails;
  // Enter Information for a student
	printf("Enter ID: ");
	scanf("%s", studentPointer->id); // Because its 'char' so it doesn't need an '&' for address
	printf("Enter first name: ");
	scanf("%s", studentPointer->firstname);
	printf("Enter last name: ");
	scanf("%s", studentPointer->lastname);
	printf("Enter Points: ");
	scanf("%f", &studentPointer->points); // Because its 'float' so it does need an '&' for address
  
  // display information entered using struct studentDetails 
	printf("\nResult via studentDetails struct\n");
      printf("ID: %s\n", studentDetails.id);
	printf("First Name: %s \n", studentDetails.firstname);
	printf("Last Name: %s \n", studentDetails.lastname);
	printf("Last Name: %s \n", studentDetails.lastname);
	printf("Points: %f\n\n",studentDetails.points);
    // code show last name
    //code show Points
    
  // display information entered via studentPointer variable
	printf("\nResult represented by studentPointer\n");
      printf("ID: %s\n", studentPointer->id);
	printf("First Name: %s \n", studentPointer->firstname);
	printf("Last Name: %s \n", studentPointer->lastname);
	printf("Points: %f\n",studentPointer->points);
    // code show last name
    //code show Points    
    
	return 0;
}
