/* Program:  roundRobinSchedulingSolution.c
 * Purpose:  Both the Ready Queue and the Job Queue are implemented as Linked Lists. 
 * Initial Job Queue: (A, 178)-->(B, 83)-->(C, 166)-->(D, 169)-->(E, 193)
 * Process is taken from the head of the Job queue (A, 178) and 
 * assigned a time slice on the CPU equal to the time quantum (20ms).
 * Process A is added to the tail of the Ready queue (A,20). As 
 * process A burst time is greater than allocated time quantum (20ms) 
 * the process is added back to the tail of the job queue.
 * Job queue now (B, 83)-->(C, 166)-->(D, 169)-->(E, 193)-->(A, 158)
 * Ready Queue now (A,20)
 * Process B taken from Job queue assigned time slice and then returned
 * to tail of Job Queue as process not completed. State of both linked lists now:
 * Job Queue: (C, 166)-->(D, 169)-->(E, 193)-->(A, 158)-->(B, 63)
 * Ready Queue: (A, 20)-->(B,20)
 *================================================================================*/

#include <stdlib.h>
#include <stdio.h>

typedef struct process {
    char processID;
    int burstTime;
    struct process *nextNode;
} node;


node *jobQueueHead = NULL; /* Job queue head */
node *jobQueueTail = NULL; /* Job queue tail */

node *readyQueueHead = NULL; /* Ready Queue head */
node *readyQueueTail = NULL; /* Ready queue's tail */

void printLinkedList(node *headOfLinkedList);


/*Function to insert Node (Process) back to the tail
* of the Job Queue - Job Queue is a circular queue
* \param jobQueueNode - node to be added to tail of
*        Job Queue 
*/

void insertNodeJobQueue(node *jobQueueNode) {
    if (jobQueueTail == NULL)  {/* empty list */
        jobQueueHead = jobQueueNode;
	}  else {
        jobQueueTail->nextNode = jobQueueNode;
	}
    jobQueueTail = jobQueueNode;
}

/* Function returns the next node to be taken from the
 * head of the Job Queue 
 */
node *removeJobQueueHeadNode() {
      node *headOfJobQueue;
      if (jobQueueHead == NULL)  {/* list empty */
            return (NULL);
            }

      
      // Extract the head of the Job Queue
      headOfJobQueue = jobQueueHead;

      // Update the Job Queue head
      if (jobQueueHead == jobQueueTail)  {/* only one element in the list */
            jobQueueHead = jobQueueTail = NULL;
      } else {
            jobQueueHead = jobQueueHead->nextNode; //replace the head with the next node of the current head
      }

      return (headOfJobQueue);
}

/* Function inserts a new node to the tail
 * of the Ready Queue representing a process
 * allocated a time slice on the CPU
 * \param newReadyQueueNode: node to add to tail of Ready Queue
 */

void insertNodeToReadyQueue(node *newReadyQueueNode) {
    if (readyQueueTail == NULL)  {/* empty list */
        readyQueueHead = newReadyQueueNode;
    } else {
        readyQueueTail->nextNode = newReadyQueueNode; // Link the existing node tail to the new node
	}
    readyQueueTail = newReadyQueueNode;
}

int main() {
    node *processRep;
    node *jobQueueProcess;
    node *readyQueueProcess;

    int i;
    int quantum=20; // time quantum

    /* initializing the ready queue for RR */
    for (i = 1; i <= 5; i++)   {
        processRep = malloc(sizeof(node));
        processRep->processID = 64 + i;
        processRep->burstTime = (int)((double)(99) * rand() / (999999999 + 1.0));
        processRep->nextNode = NULL;
        insertNodeJobQueue(processRep);
    }

    printf("The Job Queue Processes to be executed using a Round Robin Scheduling Algorithm: \n\n");
    printLinkedList(jobQueueHead);
    printf("\n\n");

    /* the RR scheduling algorithm, insert your solution here for task1 */
    
    /* Create a loop until the ready queue is empty */
    while (jobQueueHead != NULL) {
        processRep = removeJobQueueHeadNode(); // extract the head of the Job Queue to process
        if (processRep->burstTime <= quantum) { // process completes within the time quantum
            insertNodeToReadyQueue(processRep);
        } else { // process does not complete within the time quantum

            // PUSH THE REMAINING PART OF THE PROCESS BACK TO THE JOB QUEUE
            jobQueueProcess = malloc(sizeof(node)); // create a new node for the Job Queue
            jobQueueProcess->burstTime = processRep->burstTime-quantum; // update the burst time as the remaining time after the first round
            jobQueueProcess->processID = processRep->processID; // copy the process ID
            jobQueueProcess->nextNode = NULL; // set the next node to NULL
            insertNodeJobQueue(jobQueueProcess); // insert the process back to the Job Queue

            // ADD THE FINISH PART OF THE PROCESS TO THE READY QUEUE
            readyQueueProcess = malloc(sizeof(node));
            readyQueueProcess->burstTime = quantum; 
            readyQueueProcess->processID = processRep->processID;
            readyQueueProcess->nextNode = NULL;
            insertNodeToReadyQueue(readyQueueProcess);
        }
		
	}		
    
    printf("The resulting Round Robin Schedule for Process Execution: \n\n");
    printLinkedList(readyQueueHead);
    printf("\n\n");

    return 0;
}

/* Helper function to print out a linked list
 * \param headNode: the head of the linked list to be printed
 */
void printLinkedList(node *headOfLinkedList) {
    if (headOfLinkedList == NULL) {
        printf("^\n");
    } else {
        while (headOfLinkedList->nextNode)  {
            printf("(%c, %d) --> ", headOfLinkedList->processID, headOfLinkedList->burstTime);
            headOfLinkedList = headOfLinkedList->nextNode;
        }
        printf("(%c, %d) ^\n ", headOfLinkedList->processID, headOfLinkedList->burstTime);
    }
}
