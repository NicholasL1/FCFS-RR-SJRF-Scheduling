#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "list.h"
#include "scheduler.h"
#include "process.h"
#include "queue.h"

#define SIZE 100

void first_come_first_served(int num_of_processes, Queue *processQueue, Process *input);
void sort(Process *processes[], int num_of_processes);
void add_processes_at_current_time(int num_of_processes, int timer, Queue *queue, Process *input);
void check_process_completion(int num_of_processes, int *processIDs, Process *input);
int determine_CPU_timer(Process *process);

int main(int argc, char *argv[])
{
	FILE *fp;

	int num_of_processes;

	fp = fopen(argv[1], "r");
	fscanf(fp, "%d", &num_of_processes);
	if (num_of_processes > 0)
	{
		Queue *processQueue;
		processQueue = createQueue();

		Process *queue = malloc(num_of_processes * sizeof(Process));

		for (int i = 0; i < num_of_processes; i++)
		{
			fscanf(fp, "%d %d %d %d",
						 &queue[i].pid,
						 &queue[i].cpu_time,
						 &queue[i].io_time,
						 &queue[i].arrival_time);
		}

		for (int i = 0; i < num_of_processes; i++)
		{
			printf("A: %d B: %d  C: %d D: %d \n",
						 queue[i].pid,
						 queue[i].cpu_time,
						 queue[i].io_time,
						 queue[i].arrival_time);
		}

		first_come_first_served(num_of_processes, processQueue, queue);
		free(queue);
	}

	fclose(fp);

	return 0;
}

void first_come_first_served(int num_of_processes, Queue *processQueue, Process *input)
{
	// Array to hold all the process ids, if all the processes are -1 then they have all been completed
	int *processIDs;
	processIDs = (int *)malloc(num_of_processes * sizeof(int));

	for (int i = 0; i < num_of_processes; i++)
	{
		processIDs[i] = input[i].pid;
	}
	// Process holder
	Process *tempP;

	// Array for holding the processes ready to run
	Process *readyProcesses[num_of_processes];

	// Array for holding the process running (only one can run)
	Process *runningProcess[1];
	runningProcess[0] = NULL; // Initialize to NULL

	// Array for holding the processes blocked
	Process *blockedProcesses[num_of_processes];

	// Output message
	char *outputMessage = malloc(20 * num_of_processes * sizeof(char));

	int timer = 0;
	int check = 1;
	int CPUTimer = 0;
	int IOTimer = 0;
	while (check != 0)
	{
		add_processes_at_current_time(num_of_processes, timer, processQueue, input);
		if (processQueue->size > 0)
		{
			if (runningProcess[0] == NULL)
			{
				tempP = dequeue(processQueue);
				runningProcess[0] = tempP;
				CPUTimer = determine_CPU_timer(tempP);
			}
		}
		if (CPUTimer > 0) // If CPU timer is running for a process, then decrement the timer
		{
			CPUTimer--;
		}
		else // Otherwise it has ended and IO time has started, process can be removed from running array
		{
			if (tempP->cpu_time > 0)
			{
				blockedProcesses[0] = tempP; // CONVERT BLOCKEDPROCESSES INTO QUEUE
				IOTimer = tempP->io_time;
			}
			runningProcess[0] == NULL;
		}
		check_process_completion(num_of_processes, processIDs, input);
		for (int i = 0; i < num_of_processes; i++)
		{
			check = 0;
			if (processIDs[i] != -1)
			{
				check = 1;
			}
		}
		timer++;
	}

	// Free dynamically allocated memory when no longer needed
	free(outputMessage);
	free(runningProcess[0]); // Free the memory for the running process
}

void sort(Process *processes[], int num_of_processes)
{
	int swapped;
	Process *temp;

	for (int i = 0; i < num_of_processes - 1; i++)
	{
		swapped = 0; // Check if any elements were swapped in this pass

		for (int j = 0; j < num_of_processes - i - 1; j++)
		{
			// Compare processes[j] and processes[j + 1] based on some criteria
			if (processes[j]->pid > processes[j + 1]->pid)
			{
				// Swap processes[j] and processes[j + 1]
				temp = processes[j];
				processes[j] = processes[j + 1];
				processes[j + 1] = temp;

				swapped = 1; // Set the flag to true since a swap occurred
			}
		}

		// If no two elements were swapped in this pass, the array is already sorted
		if (swapped == 0)
		{
			break;
		}
	}
}

// Check if the processes have an arrival time the same as the current timer, if so add to queue
void add_processes_at_current_time(int num_of_processes, int timer, Queue *queue, Process *input)
{
	if (num_of_processes == 0 || input == NULL)
	{
		return;
	}
	// Count how many processes have an arrival time the same as timer
	int count = 0; // Initialize count

	// Create array with the size as the number found above, then add those processes to that array
	Process *process_array[num_of_processes]; // Use the maximum possible size

	for (int i = 0; i < num_of_processes; i++)
	{
		if (input[i].arrival_time == timer)
		{
			Process *process = (Process *)malloc(sizeof(Process));
			process->pid = input[i].pid;
			process->cpu_time = input[i].cpu_time;
			process->io_time = input[i].io_time;
			process->arrival_time = input[i].arrival_time;
			process_array[count] = process;
			count++; // Increment count
		}
	}

	// Sort the processes array
	sort(process_array, count); // Sort only the valid elements

	// Add the processes to the queue
	for (int i = 0; i < count; i++)
	{
		enqueue(queue, process_array[i]);
	}
}

// Check if a process does not have IO or CPU time remaining, meaning it is completed
void check_process_completion(int num_of_processes, int *processIDs, Process *input)
{
	for (int i = 0; i < num_of_processes; i++)
	{
		if (input[i].cpu_time == 0 && input[i].io_time == 0)
		{
			processIDs[i] = -1;
		}
	}
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
		// IF CPU time is even and we don't have to round up
		if (process->cpu_time % 2 == 0)
		{
			timer = process->cpu_time / 2;
		}
		else // Otherwise it is odd and we have to round up
		{
			timer = process->cpu_time / 2 + 1;
		}
	}
	process->cpu_time = process->cpu_time - timer;
	return timer;
}
