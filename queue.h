#ifndef QUEUE_H
#define QUEUE_H

// Define the structure for a queue node
typedef struct QueueNode
{
  struct Process *data; // Pointer to a Process struct
  struct QueueNode *next;
} QueueNode;

// Define the structure for the queue
typedef struct Queue
{
  QueueNode *front;
  QueueNode *rear;
  int size;
} Queue;

// Function prototypes
Queue *createQueue();
void enqueue(Queue *queue, struct Process *data);
struct Process *dequeue(Queue *queue);
int length(Queue *queue);
struct Process *peek(Queue *queue);
int isEmpty(Queue *queue);
void clearQueue(Queue *queue);

#endif