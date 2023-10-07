#include <stdio.h>
#include <stdlib.h>
#include "process.h"
#include "queue.h"

// Create a new empty queue
Queue *createQueue()
{
  Queue *queue = (Queue *)malloc(sizeof(Queue));
  if (queue == NULL)
  {
    fprintf(stderr, "Error: Unable to allocate memory for the queue.\n");
    exit(EXIT_FAILURE);
  }
  queue->front = NULL;
  queue->rear = NULL;
  queue->size = 0;
  return queue;
}

// Enqueue a new element at the rear of the queue
void enqueue(Queue *queue, struct Process *data)
{
  QueueNode *newNode = (QueueNode *)malloc(sizeof(QueueNode));
  if (newNode == NULL)
  {
    fprintf(stderr, "Error: Unable to allocate memory for a new queue node.\n");
    exit(EXIT_FAILURE);
  }
  newNode->data = data; // Store the pointer to the Process struct
  newNode->next = NULL;

  if (isEmpty(queue))
  {
    queue->front = newNode;
    queue->rear = newNode;
  }
  else
  {
    queue->rear->next = newNode;
    queue->rear = newNode;
  }

  queue->size++;
}

// Dequeue the element at the front of the queue
struct Process *dequeue(Queue *queue)
{
  if (isEmpty(queue))
  {
    fprintf(stderr, "Error: Queue is empty. Cannot dequeue.\n");
    exit(EXIT_FAILURE);
  }

  struct Process *data = queue->front->data;
  QueueNode *temp = queue->front;
  queue->front = queue->front->next;
  free(temp);

  queue->size--;

  return data;
}

// Get the number of elements in the queue
int length(Queue *queue)
{
  return queue->size;
}

// Peek at the element at the front of the queue without dequeuing
struct Process *peek(Queue *queue)
{
  if (isEmpty(queue))
  {
    fprintf(stderr, "Error: Queue is empty. Cannot peek.\n");
    exit(EXIT_FAILURE);
  }
  return queue->front->data;
}

// Check if the queue is empty
int isEmpty(Queue *queue)
{
  return queue->size == 0;
}

// Clear the entire queue and free memory
void clearQueue(Queue *queue)
{
  while (!isEmpty(queue))
  {
    dequeue(queue);
  }
  free(queue);
}