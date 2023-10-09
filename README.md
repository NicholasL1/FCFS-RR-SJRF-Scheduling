# FCFS-RR-SJRF-Scheduling

This program emulates a scheduler capable of running with three scheduling algorithms, _First Come First Served_, _Round Robin_, and _Shortest Job Remaining First_. The program takes in process information and will output the corresponding process statuses at each tick based on the scheduling, as well as the overall statistics of the execution.

To compile the code and create an executable, run:

**_gcc -o scheduler scheduler.c queue.c_**

This will create a _scheduler_ executable that you can now use to run the scheduler. To run on the command line, provide input in the following format:

**_./scheduler (input.txt) (schedule algorithm integer)_**

Where the input.txt contains the number of processes and their corresponding process information and the schedule algorithm integer is the integer 0, 1, or 2 representing which scheduling algorithm to use.

## Process.h

The process struct describes the shape of a process object when interpreted by the scheduler. Each process contains:

- int pid; _The process IO_
- int cpu_time; _The CPU time of the process_
- int io_time; _The I/O time of the process_
- int arrival_time; _The arrival time of the process_

## Queue.h / Queue.c - Queue data structure

The queue.h file represents a queue data structure which the scheduler utilizes to hold ready and blocked processes. It contains:

### QueueNode
Represents a single node of the queue
- struct Process *data; _Node's data_
- struct QueueNode *next; _The next node_

### Queue
Queue struct
- QueueNode *front; _The beginning node of the queue_
- QueueNode *rear; _The last node of the queue_
- int size; _The length of the queue_

**Functions**

- Queue *createQueue(); _Initializes a queue_
- void enqueue(Queue *queue, Process *data); _Adds element to end of queue_
- struct Process *dequeue(Queue *queue); _Takes element from the front of queue_
- struct Process *peek(Queue *queue); _Access data from the front of queue_
- int isEmpty(Queue *queue); _Check if queue is empty_
- void clearQueue(Queue *queue); _Clear the queue_ 

## Scheduler.h / Scheduler.c - Scheduler

The scheduler file takes a command line input given by a .txt file and an integer representing which scheduling algorithm to use.

**_0 - First Come First Served_**

**_1 - Round Robin_**

**_2 - Shortest Remaining Job First_**

The .txt file contains two sources of data, the first number being the number of processes the scheduler will read and the second being a graph representing those processes and their corresponding data.

The scheduler will read this input along with the specified number and evaluate the process based on which algorithm is selected, the output will be the processes and their status at each iteration.

The scheduler also gives the statistics of the scheduling, being the finishing time of the scheduling, the CPU utilization, and the turnaround times for each process.