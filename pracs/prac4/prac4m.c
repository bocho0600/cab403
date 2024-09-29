#include<stdio.h>
#include<stdlib.h>

struct Node{
    int data;
    struct Node * next;
};

void linkedListTraversal(struct Node *ptr) {
    while (ptr != NULL)  {
        printf("Element: %d\n", ptr->data);
        ptr = ptr->next;
    }
}


struct Node * insertAtEnd(struct Node *head, int data){
    struct Node * NewNode = (struct Node *)malloc(sizeof(struct Node)); //create a new node
    struct Node * temp = head; //temp is a pointer to the head node
    while(temp->next!=NULL){
        temp = temp->next;
    } //temp now points to the last node
    NewNode->data = data; //assign the data to the new node
    temp->next = NewNode; //link the last node to the new node instead of NULL
    return head;
}

int main(){
    struct Node *head;
    struct Node *second;
    struct Node *third;
    struct Node *fourth;

    // Allocate memory for nodes in the linked list in Heap
    head = malloc(sizeof(struct Node));
    second = malloc(sizeof(struct Node));
    third = malloc(sizeof(struct Node));
    fourth = malloc(sizeof(struct Node));

    // Link first and second nodes
    head->data = 7;
    head->next = second;

    // Link second and third nodes
    second->data = 11;
    second->next = third;

    // Link third and fourth nodes
    third->data = 41;
    third->next = fourth;

    // Terminate the list at the third node
    fourth->data = 66;
    fourth->next = NULL;

    printf("Linked list before insertion\n");
    linkedListTraversal(head);
	//implement insertAtFirst Function
    head =  insertAtEnd(head, 56);
    printf("\nLinked list after insertion\n");
    linkedListTraversal(head);   
    return 0;
}
