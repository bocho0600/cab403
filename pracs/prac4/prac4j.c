#include <stdio.h>

// student structure
struct student
{
	char id[15];
	char firstname[64];
	char lastname[64];
	float points;
};

// function declaration
void getDetail(struct student *);
void displayDetail(struct student *);

int main(int argc, char *argv[])
{
	struct student std[3];
	getDetail(std);
	displayDetail(std);
	return 0;
}

void getDetail(struct student *ptr)
{
	for(int i; i < 3; i++){
		printf("\n\nEnter detail of student #%d \n", i+1);
		printf("Enter ID: ");
		scanf("%s", (ptr + i)->id);
		printf("\nEnter first name: ");
		scanf("%s", (ptr + i)->firstname);
		printf("\n Enter last name: ");
		scanf("%s", (ptr + i)->lastname);
		printf("Enter Points: ");
		scanf("%f", &(ptr + 1)->points);
	}
}

void displayDetail(struct student *ptr)
{
	for(int i = 0; i < 3; i++){
		printf("Result of student #%d\n\n",i+1);
		printf("Result via ptr\n");
		printf("ID: %s\n", (ptr + i)->id);
		printf("First Name: %s\n", (ptr + i)->lastname);
		printf("Last Name: %s\n", (ptr + i)->lastname);
		printf("Points: %.2f\n",(ptr + i)->points);
	}
}
