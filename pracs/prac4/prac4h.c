#include <stdio.h>
#include <string.h>

struct myStructure {
	int studentNumber;
	char subjectGrade;
	char subjectName[30]; 
};
void OutputData(struct myStructure *s);

int main(int argc, char *argv[]) {
	struct myStructure s1;
	s1.studentNumber = 21345;
	s1.subjectGrade = 'B';
	strcpy(s1.subjectName, "CAB403");
	struct myStructure s2 = {18, 'C', "Digital Studies"}; 
	OutputData(&s1);
	OutputData(&s2); //passing the address of the structure
	return 0; 
}

void OutputData(struct myStructure *s){
	printf("Student Number: %d\n", s->studentNumber);
	printf("Subject Grade: %c\n", s->subjectGrade);
	printf("Subject Name: %s\n", s->subjectName);
}










