/**
 *  list.h
 *
 *  Full Name: Nicholas Lachhman
 *  Course section: B
 *  Description of the program: list data structure containing the tasks in the system
 *
 */

#include "process.h"

struct node
{
    Process *process;
    struct node *next;
};

// insert and delete operations.
void insert(struct node **head, Process *process);
void delete(struct node **head, Process *process);
int isEmptyList(struct node *head);
void traverse(struct node *head);
