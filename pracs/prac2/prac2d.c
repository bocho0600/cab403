#include <stdio.h>
#include <stdlib.h>

//create object like Macros for Exit Values

//Add function prototypes based on your implementation
void enterValuesIntoArr(int *ptr, int length); //A function to permit the user to enter values which are stored in the array
void printOutArray(int *ptr, int length);  //Simple function to print out the values in an array
int askUserToIncreaseArraySize(int *ptr, int length); //Function to prompt user to enter the new increased size of the array
void enterValuesIntoExistingArray(int *ptr, int CurrentLength, int newSize); //Function to add values into an existing array that is partially populated


int main(int argc, char *argv[]) {
	int numElements = 0;
	int *arrOnePtr; //There requirement for this program is a single array - there is no need to create a second array
	// ask user to determine the array size
	printf("Enter array size: ");
	scanf("%d", &numElements);
	//allocate sufficient memory and check memory allocation is successful
	//arrOnePtr = (int *)calloc(numElements * sizeof(int)); //allocate memory for the array to fit with number of element, 8 bytes per elements
	arrOnePtr = (int *)calloc(numElements, sizeof(int));
	// Enter values into the array and then print out the array values
	enterValuesIntoArr(arrOnePtr, numElements);
	printOutArray(arrOnePtr, numElements);	
	//Call function to prompt user to increase array size
	int newSize = askUserToIncreaseArraySize(arrOnePtr, numElements);
	//Populate the resized array with additional values
	enterValuesIntoExistingArray(arrOnePtr, numElements, newSize);
	//print out array - should contain values from initial creation and values entered after array is resized.
	printOutArray(arrOnePtr, newSize);	
	return 0;
}

// Function to add additional elements to a pre-existing array - the inital values stored in the array are preserved
void enterValuesIntoExistingArray(int *ptr, int CurrentLength, int newSize) {
    for(int i = CurrentLength; i < newSize; i++) {
	  printf("Enter a value for array element %d: ", i+1);
	  scanf("%d", &ptr[i]);
    }

}

int askUserToIncreaseArraySize(int *ptr, int length) {
	int additionalElements = 0;
	
	//prompt user to enter how many additional values will be required in the resized array
	printf("Enter the number of additional elements you would like to add to the array: ");
	scanf("%d", &additionalElements);
	// reallocate memory so there will be suffiicient space for both the initial array values
	ptr = (int *)realloc(ptr, (length + additionalElements) * sizeof(int)); // allocate memory for the resized array, 8 bytes per element
	// and the for the additional elements
	//always check memory allocation is successful
	return length + additionalElements;
	//return the the length of the resized array
}

//Function to add elements into an array	
void enterValuesIntoArr(int *ptr, int length) {
    for(int i = 0; i < length; i++) {
        printf("Enter a value for array element %d: ", i+1);
        scanf("%d", &ptr[i]);
    }
}

//Function to print out the values in the array
void printOutArray(int *ptr, int length) {
	for(int i = 0; i < length; i++) {
		printf("arrOnePtr[%d] = %d\n", i, ptr[i]);
	}
}

		
	
