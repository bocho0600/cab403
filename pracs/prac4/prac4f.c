#include <stdio.h>
#include <string.h>

typedef struct books {
 char title[60];
 char author[40];
 int pubYear;
} book;
void print_info(book BookName);

int main(int argc, char *argv[]) {

      book myBookOne; 
      strcpy(myBookOne.title, "CAB403"); //to assign a value to a string use strcpy() function
      strcpy(myBookOne.author, "Tommy Smith");
      myBookOne.pubYear = 2023;
      book myBookTwo = {"Learning to Program", "Jason Smith", 2019};
      print_info(myBookOne);
      print_info(myBookTwo);
}

void print_info(book BookName){
      printf("Book Information: \n");
      printf("---------------------------\n");
      printf("Book Title: %s\n", BookName.title);
      printf("Book Author: %s\n", BookName.author);
      printf("Publication Year: %d\n", BookName.pubYear);
      printf("---------------------------------------\n\n\n");
}