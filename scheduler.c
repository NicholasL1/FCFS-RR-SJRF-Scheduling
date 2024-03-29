/**
 *  scheduler.c
 *
 *  Full Name: Nicholas Lachhman
 *  Course section: B
 *  Description of the program: FCFS, RR, AND SJRF Scheduling for processes
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "scheduler.h"
#include "process.h"
#include "queue.h"

#define SIZE 100

int totalTime;
int cpuDowntime = 0;
double cpuUtil;
int *turnaroundTimes;
int *halfCPUTimes;

void first_come_first_served(int num_of_processes, Queue *processQueue, Process *input, FILE *outputFile);
void round_robin(int num_of_processes, Queue *processQueue, Process *input, FILE *outputFile);
void shortest_job_remaining_first(int num_of_processes, Queue *processQueue, Process *input, FILE *outputFile);
void add_processes_at_current_time(int num_of_processes, int timer, Queue *queue, Process *input, Queue *blockedProcesses, Queue *readyQueue);
void sort(Process *processes[], int num_of_processes);
int determine_CPU_timer(Process *process);
void decrementBlockedProcessesIO(Queue *blockedProcesses, Process *input);
int checkBlockedProcesses(Queue *blockedProcesses, Process **processArray, int index);
int compareProcessesPID(const void *a, const void *b);
void formatOutputMessage(int num_of_processes, int timer, Queue *blockedProcesses, Queue *readyQueue, Process *runningProcess, Queue *readyQueue2, char *outputMessage);
int check_process_completion(int num_of_processes, Process *input);
int determine_CPU_timer_RR(Process *process);
char *removeFileExtension(char *filename);
void sortQueueByCpuTime(Queue *queue, Process *runningProcess);
int compareProcessesCPU(const void *a, const void *b);

int main(int argc, char *argv[])
{
	FILE *fp;

	argc = argc;

	int num_of_processes;
	int algo;

	fp = fopen(argv[1], "r");
	fscanf(fp, "%d", &num_of_processes);

	char *inputFileName = argv[1];
	algo = atoi(argv[2]);

	// Remove the ".txt" extension from the input file name
	char *baseInputFileName = removeFileExtension(inputFileName);

	// Concatenate '-x.txt' to the base input file name where x is the third argument
	char *outputFileNameWithSuffix = (char *)malloc(strlen(baseInputFileName) + 7);
	sprintf(outputFileNameWithSuffix, "%s-%s.txt", baseInputFileName, argv[2]);

	if (num_of_processes > 0)
	{
		Queue *processQueue = createQueue();
		Process *queue = malloc(num_of_processes * sizeof(Process));

		for (int i = 0; i < num_of_processes; i++)
		{
			fscanf(fp, "%d %d %d %d",
						 &queue[i].pid,
						 &queue[i].cpu_time,
						 &queue[i].io_time,
						 &queue[i].arrival_time);
		}
		// Open the output file for writing
		FILE *outputFile = fopen(outputFileNameWithSuffix, "w");

		if (outputFile == NULL)
		{
			perror("Error opening output file");
			free(baseInputFileName);
			free(outputFileNameWithSuffix);
			return 1;
		}

		if (algo == 0)
		{
			first_come_first_served(num_of_processes, processQueue, queue, outputFile);
			cpuUtil = (double)(totalTime - (cpuDowntime - 1)) / totalTime;
			fprintf(outputFile, "\nFinishing time: %d\n", totalTime - 1);
			fprintf(outputFile, "CPU Utilization: %.2f\n", cpuUtil);
			for (int i = 0; i < num_of_processes; i++)
			{
				fprintf(outputFile, "Turnaround Process %d: %d\n", i, turnaroundTimes[i]);
			}
		}
		else if (algo == 1)
		{
			round_robin(num_of_processes, processQueue, queue, outputFile);
			cpuUtil = (double)(totalTime - (cpuDowntime - 1)) / totalTime;
			fprintf(outputFile, "\nFinishing time: %d\n", totalTime - 1);
			fprintf(outputFile, "CPU Utilization: %.2lf\n", cpuUtil);
			for (int i = 0; i < num_of_processes; i++)
			{
				fprintf(outputFile, "Turnaround Process %d: %d\n", i, turnaroundTimes[i]);
			}
		}
		else if (algo == 2)
		{
			shortest_job_remaining_first(num_of_processes, processQueue, queue, outputFile);
			cpuUtil = (double)(totalTime - (cpuDowntime - 1)) / totalTime;
			fprintf(outputFile, "\nFinishing time: %d\n", totalTime - 1);
			fprintf(outputFile, "CPU Utilization: %.2lf\n", cpuUtil);
			for (int i = 0; i < num_of_processes; i++)
			{
				fprintf(outputFile, "Turnaround Process %d: %d\n", i, turnaroundTimes[i]);
			}
		}
		free(queue);
	}

	fclose(fp);

	return 0;
}

void shortest_job_remaining_first(int num_of_processes, Queue *processQueue, Process *input, FILE *outputFile)
{
	// Initialize global array for storing turnaround times
	turnaroundTimes = (int *)malloc(num_of_processes * sizeof(int));

	// Output message
	char *outputMessage = malloc(20 * num_of_processes * sizeof(char));

	// Process holder
	Process *tempP = (Process *)malloc(sizeof(Process));

	// Queue for holding blocked processes
	Queue *blockedProcesses;
	blockedProcesses = createQueue();

	// Filler queue - used in RR
	Queue *readyQueue;
	readyQueue = createQueue();

	// Array for holding the process running (only one can run)
	Process *runningProcess[1];
	runningProcess[0] = NULL; // Initialize to NULL

	int check = 1;
	int timer = 0;
	int burstTimer = 0;
	while (check != 0)
	{
		add_processes_at_current_time(num_of_processes, timer, processQueue, input, blockedProcesses, readyQueue);
		if (processQueue->size > 0)
		{
			sortQueueByCpuTime(processQueue, runningProcess[0]);
			if (runningProcess[0] == NULL)
			{
				tempP = dequeue(processQueue);
				runningProcess[0] = tempP;
				burstTimer = determine_CPU_timer(tempP);
				input[tempP->pid].cpu_time = input[tempP->pid].cpu_time - burstTimer;
			}
		}
		// Check if there is no process running
		if (runningProcess[0] == NULL)
		{
			cpuDowntime++;
		}
		decrementBlockedProcessesIO(blockedProcesses, input);

		// Format and print output message for this timer iteration
		formatOutputMessage(num_of_processes, timer, blockedProcesses, processQueue, runningProcess[0], readyQueue, outputMessage);
		if (strlen(outputMessage) < 4)
		{
			if (check_process_completion(num_of_processes, input) != 0)
			{
				totalTime = timer;
				break;
			}
		}
		fprintf(outputFile, "%s\n", outputMessage); // Print output message

		if (burstTimer > 0) // If CPU timer is running for a process, then decrement the timer
		{
			burstTimer--;
			if (burstTimer == 0) // It has ended and IO time has started, process can be removed from running array
			{
				tempP = runningProcess[0];
				if (tempP->cpu_time > 0)
				{
					enqueue(blockedProcesses, tempP);
				}
				else
				{
					input[tempP->pid].cpu_time = 0;
					turnaroundTimes[tempP->pid] = timer - tempP->arrival_time + 1;
				}
				runningProcess[0] = NULL;
			}
		}
		timer++;
	}
	free(outputMessage);
	free(runningProcess[0]);
}

// Function to remove the file extension from a filename
char *removeFileExtension(char *filename)
{
	char *dot = strrchr(filename, '.');
	if (!dot || dot == filename)
	{
		return strdup(filename); // No extension found, return a copy of the original filename
	}
	else
	{
		size_t len = dot - filename;
		char *newFilename = (char *)malloc(len + 1);
		if (newFilename)
		{
			memcpy(newFilename, filename, len);
			newFilename[len] = '\0';
		}
		return newFilename;
	}
}

// Round robin scheduling method
void round_robin(int num_of_processes, Queue *processQueue, Process *input, FILE *outputFile)
{
	// Initialize global array for storing turnaround times
	turnaroundTimes = (int *)malloc(num_of_processes * sizeof(int));

	// Initialize global array for storing half the CPU times for initial processing
	halfCPUTimes = (int *)malloc(num_of_processes * sizeof(int));

	for (int i = 0; i < num_of_processes; i++)
	{
		Process *newP = (Process *)malloc(sizeof(Process));
		newP->pid = input[i].pid;
		newP->cpu_time = input[i].cpu_time;
		newP->io_time = input[i].io_time;
		newP->arrival_time = input[i].arrival_time;
		halfCPUTimes[i] = determine_CPU_timer(newP);
		free(newP);
	}

	// Output message
	char *outputMessage = malloc(20 * num_of_processes * sizeof(char));

	// Process holder
	Process *tempP = (Process *)malloc(sizeof(Process));

	// Queue for holding blocked processes
	Queue *blockedProcesses;
	blockedProcesses = createQueue();

	// Queue for holding ready processes that aren't blocked
	Queue *readyQueue;
	readyQueue = createQueue();

	// Array for holding the process running (only one can run)
	Process *runningProcess[1];
	runningProcess[0] = NULL; // Initialize to NULL

	int check = 1;
	int timer = 0;
	int burstTimer = 0;
	while (check != 0)
	{
		add_processes_at_current_time(num_of_processes, timer, processQueue, input, blockedProcesses, readyQueue);
		if (processQueue->size > 0)
		{
			if (runningProcess[0] == NULL)
			{
				tempP = dequeue(processQueue);
				runningProcess[0] = tempP;
				burstTimer = determine_CPU_timer_RR(tempP);
				input[tempP->pid].cpu_time = input[tempP->pid].cpu_time - burstTimer;
			}
		}
		// Check if there is no process running
		if (runningProcess[0] == NULL)
		{
			cpuDowntime++;
		}
		decrementBlockedProcessesIO(blockedProcesses, input);

		// Format and print output message for this timer iteration
		formatOutputMessage(num_of_processes, timer, blockedProcesses, processQueue, runningProcess[0], readyQueue, outputMessage);
		if (strlen(outputMessage) < 4)
		{
			if (check_process_completion(num_of_processes, input) != 0)
			{
				totalTime = timer;
				break;
			}
		}
		fprintf(outputFile, "%s\n", outputMessage); // Print output message

		if (burstTimer > 0) // If CPU timer is running for a process, then decrement the timer
		{
			burstTimer--;
			if (burstTimer == 0) // It has ended and IO time has started, process can be removed from running array
			{
				tempP = runningProcess[0];
				if (tempP->cpu_time > 0)
				{
					if (halfCPUTimes[tempP->pid] == 0 && tempP->io_time > 0)
					{
						enqueue(blockedProcesses, tempP);
					}
					else
					{
						enqueue(readyQueue, tempP);
					}
				}
				else
				{
					input[tempP->pid].cpu_time = 0;
					turnaroundTimes[tempP->pid] = timer - tempP->arrival_time + 1;
				}
				runningProcess[0] = NULL;
			}
		}
		timer++;
	}
	free(outputMessage);
	free(runningProcess[0]);
}

void first_come_first_served(int num_of_processes, Queue *processQueue, Process *input, FILE *outputFile)
{
	// Initialize global array for storing turnaround times
	turnaroundTimes = (int *)malloc(num_of_processes * sizeof(int));

	// Output message
	char *outputMessage = malloc(20 * num_of_processes * sizeof(char));

	// Process holder
	Process *tempP = (Process *)malloc(sizeof(Process));

	// Queue for holding blocked processes
	Queue *blockedProcesses;
	blockedProcesses = createQueue();

	// Filler queue
	Queue *readyQueue;
	readyQueue = createQueue();

	// Array for holding the process running (only one can run)
	Process *runningProcess[1];
	runningProcess[0] = NULL; // Initialize to NULL

	int check = 1;
	int timer = 0;
	int burstTimer = 0;
	while (check != 0)
	{
		add_processes_at_current_time(num_of_processes, timer, processQueue, input, blockedProcesses, readyQueue);
		if (processQueue->size > 0)
		{
			if (runningProcess[0] == NULL)
			{
				tempP = dequeue(processQueue);
				runningProcess[0] = tempP;
				burstTimer = determine_CPU_timer(tempP);
				input[tempP->pid].cpu_time = input[tempP->pid].cpu_time - burstTimer;
			}
		}
		// Check if there is no process running
		if (runningProcess[0] == NULL)
		{
			cpuDowntime++;
		}
		decrementBlockedProcessesIO(blockedProcesses, input);

		// Format and print output message for this timer iteration
		formatOutputMessage(num_of_processes, timer, blockedProcesses, processQueue, runningProcess[0], readyQueue, outputMessage);
		if (strlen(outputMessage) < 4)
		{
			if (check_process_completion(num_of_processes, input) != 0)
			{
				totalTime = timer;
				break;
			}
		}
		fprintf(outputFile, "%s\n", outputMessage); // Print output message

		if (burstTimer > 0) // If CPU timer is running for a process, then decrement the timer
		{
			burstTimer--;
			if (burstTimer == 0) // It has ended and IO time has started, process can be removed from running array
			{
				tempP = runningProcess[0];
				if (tempP->cpu_time > 0)
				{
					enqueue(blockedProcesses, tempP);
				}
				else
				{
					input[tempP->pid].cpu_time = 0;
					turnaroundTimes[tempP->pid] = timer - tempP->arrival_time + 1;
				}
				runningProcess[0] = NULL;
			}
		}
		timer++;
	}
	free(outputMessage);
	free(runningProcess[0]);
}

// Check if the processes have an arrival time the same as the current timer, if so add to queue
void add_processes_at_current_time(int num_of_processes, int timer, Queue *queue, Process *input, Queue *blockedProcesses, Queue *readyQueue)
{
	if (num_of_processes == 0 || input == NULL)
	{
		return;
	}
	// Count how many processes have an arrival time the same as timer
	int count = 0;

	// Create array with the size as the number found above, then add those processes to that array
	Process *processArray[num_of_processes]; // Use the maximum possible size

	for (int i = 0; i < num_of_processes; i++)
	{
		if (input[i].arrival_time == timer)
		{
			Process *process = (Process *)malloc(sizeof(Process));
			process->pid = input[i].pid;
			process->cpu_time = input[i].cpu_time;
			process->io_time = input[i].io_time;
			process->arrival_time = input[i].arrival_time;
			processArray[count] = process;
			count++; // Increment count
		}
	}
	// Add any blocked processes that are ready into process array
	count = checkBlockedProcesses(blockedProcesses, processArray, count);

	// Add any ready processes from the readyQueue to the process array
	while (isEmpty(readyQueue) == 0)
	{
		processArray[count] = dequeue(readyQueue);
		count++;
	}

	// Sort the processes array
	sort(processArray, count); // Sort only the valid elements
	// Add the processes to the queue
	for (int i = 0; i < count; i++)
	{
		enqueue(queue, processArray[i]);
	}
}

// Helper method for sorting by PID
int compareProcessesPID(const void *a, const void *b)
{
	// Compare two processes based on their pid field
	return ((Process *)a)->pid - ((Process *)b)->pid;
}

// Sort array of processes
void sort(Process *processes[], int num_of_processes)
{
	// Check for valid input
	if (processes == NULL || num_of_processes <= 0)
	{
		return;
	}

	// Use the qsort function from the standard library to sort the array of pointers
	qsort(processes, num_of_processes, sizeof(Process *), compareProcessesPID);
}

// Method to determine what the value of the CPU timer should be
int determine_CPU_timer(Process *process)
{
	int timer = 0;
	if (process->io_time == 0) // If IO time is completed, then we set CPU timer to the remaining CPU time
	{
		timer = process->cpu_time;
	}
	else
	{
		if (process->cpu_time == 1) // If CPU time is 1
		{
			timer = 1;
		}
		// IF CPU time is even and we don't have to round up
		else if (process->cpu_time % 2 == 0)
		{
			timer = process->cpu_time / 2;
		}
		else if (process->cpu_time % 2 == 1) // Otherwise it is odd and we have to round up
		{
			timer = process->cpu_time / 2 + 1;
		}
	}
	process->cpu_time = process->cpu_time - timer;
	return timer;
}

// Decrement the blocked processes IO time
void decrementBlockedProcessesIO(Queue *blockedProcesses, Process *input)
{
	// Create a temporary queue to store processes that will be processed
	Queue *tempQueue = createQueue();

	// Dequeue each process and decrement IO
	while (isEmpty(blockedProcesses) == 0)
	{
		Process *process = dequeue(blockedProcesses);
		process->io_time--;
		input[process->pid].io_time = input[process->pid].io_time - 1;
		enqueue(tempQueue, process);
	}

	// Move the processes back from the temporary queue to the blockedProcesses queue
	while (isEmpty(tempQueue) == 0)
	{
		enqueue(blockedProcesses, dequeue(tempQueue));
	}

	free(tempQueue);
}

// Check if the blockedProcesses IO time is 0
int checkBlockedProcesses(Queue *blockedProcesses, Process **processArray, int index)
{
	// Create a temp queue
	Queue *tempQueue = createQueue();

	// Dequeue each process and determine if IO time is 0
	while (isEmpty(blockedProcesses) == 0)
	{
		Process *process = dequeue(blockedProcesses);

		if (process->io_time == 0) // If IO time is 0, add it to process array
		{
			Process *newProcess = (Process *)malloc(sizeof(Process)); // Allocate a new Process struct
			if (newProcess == NULL)
			{
				fprintf(stderr, "Error: Unable to allocate memory for a new process.\n");
				exit(EXIT_FAILURE);
			}

			// Copy the data from the original process to the new one
			newProcess->pid = process->pid;
			newProcess->cpu_time = process->cpu_time;
			newProcess->io_time = process->io_time;
			newProcess->arrival_time = process->arrival_time;

			processArray[index] = newProcess; // Add the new process to processArray
			index++;
		}
		else // Otherwise, add it to temp queue
		{
			enqueue(tempQueue, process);
		}
	}

	// Move processes in tempQueue back to blockedProcesses
	while (isEmpty(tempQueue) == 0)
	{
		Process *p = dequeue(tempQueue);
		enqueue(blockedProcesses, p);
	}
	return index;
}

void formatOutputMessage(int num_of_processes, int timer, Queue *blockedProcesses, Queue *readyQueue, Process *runningProcess, Queue *readyQueue2, char *outputMessage)
{
	Queue *tempQueue = createQueue();

	sprintf(outputMessage, "%d ", timer);
	// Create ID array and initialize all values to ""
	char activeIDs[num_of_processes][10];
	for (int i = 0; i < num_of_processes; i++)
	{
		strcpy(activeIDs[i], "");
	}

	// Getting ID of running process
	if (runningProcess != NULL)
	{
		strcpy(activeIDs[runningProcess->pid], "running");
	}

	// Getting the IDs of the blocked processes
	while (isEmpty(blockedProcesses) == 0)
	{
		Process *process = dequeue(blockedProcesses);
		strcpy(activeIDs[process->pid], "blocked");
		enqueue(tempQueue, process);
	}
	while (isEmpty(tempQueue) == 0) // Adding the processes back to blockedProcesses
	{
		enqueue(blockedProcesses, dequeue(tempQueue));
	}

	// Repeating same process for both readyProcesses queue
	while (isEmpty(readyQueue) == 0)
	{
		Process *process2 = dequeue(readyQueue);
		strcpy(activeIDs[process2->pid], "ready");
		enqueue(tempQueue, process2);
	}
	while (isEmpty(tempQueue) == 0)
	{
		enqueue(readyQueue, dequeue(tempQueue));
	}

	while (isEmpty(readyQueue2) == 0)
	{
		Process *process3 = dequeue(readyQueue2);
		strcpy(activeIDs[process3->pid], "ready");
		enqueue(tempQueue, process3);
	}
	while (isEmpty(tempQueue) == 0)
	{
		enqueue(readyQueue2, dequeue(tempQueue));
	}
	free(tempQueue);

	// Loop to format the output message:
	char *processStatuses = malloc(20 * num_of_processes * sizeof(char));

	for (int i = 0; i < num_of_processes; i++)
	{
		if (strcmp(activeIDs[i], "") != 0)
		{
			char temp[20];
			snprintf(temp, sizeof(temp), "%d: %s ", i, activeIDs[i]);
			strcat(processStatuses, temp);
		}
	}
	sprintf(outputMessage, "%d %s", timer, processStatuses);
}

// Check if a input still has remaining cpu time, then we cannot break out the while loop
int check_process_completion(int num_of_processes, Process *input)
{
	for (int i = 0; i < num_of_processes; i++)
	{
		if (input[i].cpu_time != 0)
		{
			return 0;
		}
	}
	return 1;
}

// Method to determine what the value of the CPU timer should be with Round Robin algorithm
int determine_CPU_timer_RR(Process *process)
{
	int timer = 0;
	if (halfCPUTimes[process->pid] == 0)
	{
		if (process->cpu_time < 2)
		{
			timer = 1;
		}
		else
		{
			timer = 2;
		}
	}
	else
	{
		if (halfCPUTimes[process->pid] < 2)
		{
			timer = 1;
		}
		else
		{
			timer = 2;
		}
	}
	// Decrement process and halfTime remaining
	process->cpu_time = process->cpu_time - timer;
	halfCPUTimes[process->pid] = halfCPUTimes[process->pid] - timer;
	return timer;
}

// Sorts a queue by CPU time and determines if the runningProcess should be replaced
void sortQueueByCpuTime(Queue *queue, Process *runningProcess)
{
	int queueSize = queue->size;
	if (queueSize <= 1)
	{
		return; // Nothing to sort
	}
	// Create a temporary queue to hold the sorted elements
	Queue *tempQueue = createQueue();

	// Create an array of Process pointers to hold the elements temporarily
	Process **processArray = (Process **)malloc(queueSize * sizeof(Process *));
	if (processArray == NULL)
	{
		fprintf(stderr, "Memory allocation failed.\n");
		exit(EXIT_FAILURE); // Handle memory allocation failure
	}
	// Dequeue all elements from the original queue and store them in the array
	for (int i = 0; i < queueSize; i++)
	{
		processArray[i] = dequeue(queue);
	}

	// Sort the array based on cpu_time and pid
	qsort(processArray, queueSize, sizeof(Process *), compareProcessesCPU);

	// If runningProcess is not NULL, find the process with the lowest cpu_time
	if (runningProcess != NULL)
	{
		Process *lowestCpuTimeProcess = processArray[0];
		if (runningProcess->cpu_time > lowestCpuTimeProcess->cpu_time)
		{
			// Replace runningProcess with the one with the lowest cpu_time
			enqueue(tempQueue, runningProcess);		 // Add the previous runningProcess to the temporary queue
			runningProcess = lowestCpuTimeProcess; // Update runningProcess
		}
	}

	// Enqueue the sorted elements back into the original queue
	for (int i = 0; i < queueSize; i++)
	{
		enqueue(queue, processArray[i]);
	}
	// Free the temporary array
	free(processArray);
}

// Comparison function for CPU time
int compareProcessesCPU(const void *a, const void *b)
{
	Process *processA = *(Process **)a;
	Process *processB = *(Process **)b;

	// First, compare by cpu_time
	int cpuTimeComparison = processA->cpu_time - processB->cpu_time;

	if (cpuTimeComparison != 0)
	{
		return cpuTimeComparison; // Sort by cpu_time if they are different
	}
	else
	{
		// If cpu_time is the same, compare by pid
		return processA->pid - processB->pid;
	}
}