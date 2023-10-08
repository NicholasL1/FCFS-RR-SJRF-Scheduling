/**
 *  list.c
 *
 *  Full Name: Nicholas Lachhman
 *  Course section: B
 *  Description of the program: Various list operations
 *
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "list.h"
#include "process.h"

// add a new node to the list
void insert(struct node **head, Process *newProcess)
{
    // add the new process to the list
    struct node *newNode = malloc(sizeof(struct node));

    newNode->process = newProcess;
    newNode->next = *head;
    *head = newNode;
}

// delete the selected now from the list
void delete(struct node **head, Process *process)
{
    struct node *temp;
    struct node *prev;

    temp = *head;
    // special case - beginning of list
    if (process->pid == temp->process->pid)
    {
        *head = (*head)->next;
    }
    else
    {
        // interior or last element in the list
        prev = *head;
        temp = temp->next;
        while ((process->pid != temp->process->pid))
        {
            prev = temp;
            temp = temp->next;
        }

        prev->next = temp->next;
    }
}

// Check if the list is empty
int IsEmptyList(struct node *head)
{
    return head == NULL; // 1 if empty, 0 if not empty
}

// traverse the list
void traverse(struct node *head)
{
    struct node *temp;
    temp = head;

    while (temp != NULL)
    {
        printf("[%d] [%d]\n", temp->process->pid, temp->process->cpu_time);
        temp = temp->next;
    }
}
