//Author - Harminder Singh Hps251@nyu.edu

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct process
{
	int pid;
	int arrivalTime;
	int burstTime;
	int cpuTime;
	int ioTime;
	int priority;
	int ready;
	int waiting;
	int waitingTime;
	int terminated;
	int recentlyAdded;
	
	int completed;
	int waited;
	int totalWaited;
	int numRan;
	int burstVal;
};

//Queue found online at http://www.sanfoundry.com/c-program-queue-using-linked-list
//Modified for using struct processes
//Queue count
int count = 0;
struct node {
	struct process proc;
	struct node *pointer;
}*front, *rear, *temp, *front1;

void create() 
{
	front = rear = NULL;
}

void queueSize() 
{
	printf("Queue size: %d\n", count);
}

void enqueue(struct process proc)
{
	if(rear == NULL) 
	{
		rear = (struct node *)malloc(1 * sizeof(struct node));
		rear->pointer = NULL;
		rear->proc = proc;
		front = rear;
	}
	else
	{
		temp = (struct node *)malloc(1 * sizeof(struct node));
		rear->pointer = temp;
		temp->proc = proc;
		temp->pointer = NULL;
		rear = temp;
	}
	count++;
}

void dequeue()
{
		front1 = front;
		if(front1 == NULL) 
		{
			printf("Error: Trying toq ueue from empty queue\n");
			return;
		}
		else {
			if(front1->pointer != NULL) 
			{
				front1 = front1->pointer;
				free(front);
				front = front1;
			}
			else 
			{
				free(front);
				front = NULL;
				rear = NULL;
			}
			count--;
		}
}

struct process frontElement() 
{
	if((front != NULL) && (rear != NULL)) 
	{
		return (front->proc);
	}
}

int empty() 
{
	if((front != NULL) && (rear != NULL))
	{
		return 1;
	}
	else 
	{
		return 0;
	}
}

int compare(const void *arg1, const void *arg2) {
	struct process *proc1 = (struct process *)arg1;
	struct process *proc2 = (struct process *)arg2;
	
	int comp = proc1->arrivalTime - proc2->arrivalTime;
	return comp;
	
}

int sjfc(const void *arg1, const void *arg2) {
	struct process *proc1 = (struct process *)arg1;
	struct process *proc2 = (struct process *)arg2;
	
	int comp = (proc1->cpuTime - proc1->numRan) - (proc2->cpuTime - proc2->numRan);
	return comp;
	
}

int pid(const void *arg1, const void *arg2) {
	struct process *proc1 = (struct process *)arg1;
	struct process *proc2 = (struct process *)arg2;
	
	int comp = proc1->pid - proc2->pid;
	return comp;
	
}

int randomOS(int cpuBurst, FILE* file, int verbose){
	char oneword[1024];
	char c;
	c = fscanf(file, "%s", oneword);
	int value = atoi(oneword);
	if(verbose == 1){
		printf("Find burst when choosing ready process to run %d\n", value);	
	}
	 
	return 1 + ( value % cpuBurst);
}

void rr(struct process processes[], char * fileName, int size, int verbose, struct process origProcesses[]) 
{
	FILE *numFile;
	numFile = fopen(fileName, "r");
	if(numFile != NULL) {
		int finish = 0;
		int etime = 0;
		int procRunning = 0;
		create();
		int waitingSize = 0;
		int k, l, i;
		int index;
		struct process currProc;
		int queue[size];
		for(i = 0; i < size; i++) {
			queue[i] = 0;
		}
		//Add processes to ready queue
		for(k = 0; k < size; k++) {
			processes[k].pid = k;
			if(processes[k].arrivalTime == etime) {
				processes[k].ready = 1;
				enqueue(processes[k]);
			}
		}
		
		while( finish < size ){
			
			if(procRunning == 0 && empty() != 0) {
				currProc = frontElement();
				for(i = 0; i < size; i++) {
					if(processes[i].pid == currProc.pid) {
						index = i;
						break;
					}
				}
				processes[index].ready = 2;
				procRunning = 1;
				dequeue();
			}
			
			if(procRunning == 0 && empty() == 0) 
			{
				if(waitingSize > 0) {
					int bool = 0;
					for(l = 0; l < size; l++) {
						if(processes[l].waiting == 1) {
							processes[l].recentlyAdded++;
						}
						if(processes[l].waiting == 1 && processes[l].recentlyAdded > 1) {
							bool = 1;
						}
					}
					if(bool == 1) {
						etime++;
					}
					if(verbose == 1) {
						printf("Before cycle %d: ", etime);
						for(i = 0; i < size; i++) {
							if(processes[i].cpuTime > 0) {
								if(processes[i].ready == 2) {
									printf(" running 0 " );
								}
								if(processes[i].ready == 1 && processes[i].waiting != 1 && processes[i].arrivalTime != etime) {
									printf(" ready 0 ");
								}
								if(processes[i].recentlyAdded > 1) {
									printf(" blocked %d ", processes[i].waitingTime);
								}
								if(processes[i].ready == -1 || processes[i].ready == 1 && processes[i].arrivalTime == etime) {
									printf(" unstarted 0 ");
								}
							}
							else {
								printf(" terminated 0 ");
							}
						}
						printf("\n");
					}
					
					for(l = 0; l < size; l++) {
						if(processes[l].waiting == 1 && processes[l].recentlyAdded > 1) {
							processes[l].waitingTime--;
							processes[l].totalWaited +=1;
							if(processes[l].waitingTime == 0) {
								processes[l].waiting = 0;
								processes[l].ready = 1;
								processes[l].recentlyAdded = 0;
								waitingSize--;
								enqueue(processes[l]);
							}
						}
					}
				
					for(k = 0; k < size; k++) 
					{
						if(processes[k].arrivalTime == etime) 
						{
							processes[k].ready = 1;
							enqueue(processes[k]);
						}
					}
					procRunning = 0;
				}
			}
			
			if(procRunning == 1) {
				if(processes[index].burstVal == 0) {
					int getBurst = randomOS(processes[index].burstTime, numFile, verbose);
					processes[index].burstVal = getBurst;
					processes[index].waitingTime = getBurst * processes[index].ioTime;
					
					if(getBurst > processes[index].cpuTime) {
						processes[index].cpuTime = getBurst;
					}
				}
				
				int quant = 2;
				procRunning = 0;
				while(quant > 0) {
					etime++;
					
					for(i = 0; i < size; i++) {
						if(processes[i].waiting == 1) {
							processes[i].recentlyAdded++;
						}
					}
					
					if(verbose == 1) {
						printf("Before cycle %d: ", etime);
						for(i = 0; i < size; i++) {
							if(processes[i].cpuTime > 0) {
								if(processes[i].ready == 2) {
									if(processes[i].burstVal >= 2) {
										printf(" running %d ", quant );
									}
									else {
										printf(" running 1 ");
									}
								}
								if(processes[i].ready == 1 && processes[i].waiting != 1 && processes[i].arrivalTime != etime) {
									printf(" ready 0 ");
								}
								if(processes[i].recentlyAdded > 1) {
									printf(" blocked %d ", processes[i].waitingTime);
								}
								if(processes[i].ready == -1 || processes[i].ready == 1 && processes[i].arrivalTime == etime) {
									printf(" unstarted 0 ");
								}
							}
							else {
								printf(" terminated 0 ");
							}
						}
						printf("\n");
					}
					
					for(i = 0; i < size; i++) {
						if(processes[i].ready == 1) {
							processes[i].numRan += 1;
						}
					}
					
					processes[index].cpuTime--;
					processes[index].burstVal--;
					quant--;
					if(waitingSize > 0) {
						for(i = 0; i < size; i++) {
							if(processes[i].waiting == 1 && processes[i].recentlyAdded > 1) {
								processes[i].waitingTime--;
								processes[i].totalWaited += 1;
								if(processes[i].waitingTime == 0) {
									processes[i].ready = 1;
									processes[i].waiting = 0;
									processes[i].recentlyAdded = 0;
									waitingSize--;
									//enqueue(processes[i]);
									queue[i] = 1;
								}
							}
						}
					}
					
					if(processes[index].cpuTime > 0 && processes[index].burstVal > 0 && quant > 0) {
						for(i = 0; i < size; i++) {
							if(queue[i] == 1) {
								queue[i] = 0;
								enqueue(processes[i]);
							}
						}
					}
					
					if(processes[index].cpuTime > 0 && processes[index].burstVal > 0 && quant == 0) {
						processes[index].ready = 1;
						for(i = 0; i < size; i++) {
							if(i == index) {
								enqueue(processes[index]);
							}
							if(i > index || i < index) {
								if(queue[i] == 1) {
									queue[i] = 0;
									enqueue(processes[i]);
								}
							}
						}
						for(i = 0; i < size; i++) {
							if(processes[i].arrivalTime == etime) {
								processes[i].ready = 1;
								enqueue(processes[i]);
							}
						}
						break;
					}
					
					if(processes[index].cpuTime > 0 && processes[index].burstVal == 0 && quant >= 0) {
						processes[index].ready = 0;
						processes[index].waiting = 1;
						processes[index].recentlyAdded = 1;
						waitingSize++;
						for(i = 0; i < size; i++) {
							if(queue[i] == 1) {
								queue[i] = 0;
								enqueue(processes[i]);
							}
						}
						for(i = 0; i < size; i++) {
							if(processes[i].arrivalTime == etime) {
								processes[i].ready = 1;
								enqueue(processes[i]);
							}
						}
						break;
					}
					
					if(processes[index].cpuTime == 0) {
						finish++;
						processes[index].completed = etime;
						for(i = 0; i < size; i++) {
							if(queue[i] == 1) {
								queue[i] = 0;
								enqueue(processes[i]);
							}
						}
						for(i = 0; i < size; i++) {
							if(processes[i].arrivalTime == etime) {
								processes[i].ready = 1;
								enqueue(processes[i]);
							}
						}
						break;
					}
					for(i = 0; i < size; i++) {
						if(processes[i].arrivalTime == etime) {
							processes[i].ready = 1;
							enqueue(processes[i]);
						}
					}
				}
			}		
		}
		
		printf("The scheduling algorithm used was Round Robin\n\n");
		qsort(processes, size, sizeof(struct process), pid);
		float cpuUtil = 0;
		float ioAVG = 0;
		float turnaroundAVG = 0;
		float avgWT = 0;
		for(i = 0; i < size; i++) {
			printf("Process %d:\n", i);
			printf("          (A,B,C,M) = (%d,%d,%d,%d)\n", origProcesses[i].arrivalTime, origProcesses[i].burstTime, origProcesses[i].cpuTime, origProcesses[i].ioTime);
			printf("          Finishing time: %d\n", processes[i].completed);
			printf("          Turnaround time: %d\n", processes[i].completed - origProcesses[i].arrivalTime);
			turnaroundAVG += (processes[i].completed - origProcesses[i].arrivalTime);
			printf("          I/O time: %d\n", processes[i].totalWaited);
			printf("          Waiting time: %d\n", processes[i].numRan);
			avgWT += processes[i].numRan;
			cpuUtil += origProcesses[i].cpuTime;
			ioAVG += processes[i].totalWaited;
		}
		
		turnaroundAVG = turnaroundAVG / size;
		avgWT = avgWT / size;
		float throughput = size * 100.0;
		throughput = throughput / etime;
		
		cpuUtil = cpuUtil / etime;
		ioAVG = ioAVG / etime;
		
		printf("\n");
		printf("Summary Data: \n");
		printf("          Finishing time: %d\n", etime);
		printf("          CPU Utilization: %f\n", cpuUtil);
		printf("          I/O Utilization: %f\n", ioAVG);
		printf("          Throughput: %f processes per hundred cycles\n", throughput);
		printf("          Average turnaround time: %f\n", turnaroundAVG);
		printf("          Average waiting time: %f\n", avgWT);
	}
	else {
		printf("File %s could not be opened\n", fileName);
		exit(0);
	}
}

void sjf(struct process processes[], char * fileName, int size, int verbose, struct process origProcesses[]) 
{
	FILE *numFile;
	numFile = fopen(fileName, "r");
	if(numFile != NULL) {
		int finish = 0;
		int etime = 0;
		int procRunning = 0;
		int burstPeriod = 0;
		int waitingSize;
		int index = 0;
		
		struct process sorted[size];
		
		//Pick job
		int i;
		qsort(processes, size, sizeof(struct process), compare);
		printf("The (sorted) input is:  %d ", size);
		for(i = 0; i < size; i++) {
			printf("( %d %d %d %d ) ", processes[i].arrivalTime, processes[i].burstTime, processes[i].cpuTime, processes[i].ioTime);
		}
		qsort(processes, size, sizeof(struct process), sjfc);
		printf("\n\n");
		
		if(verbose == 1) {
			printf("This detailed printout gives the state and remaining burst for each process\n\n");
			printf("Before cycle 0: ");
			for(i = 0; i < size; i++) 
			{
				printf("unstarted 0 ");
			}
			printf("\n");
		}
		
		for(i = 0; i < size; i++) {
			if(processes[i].arrivalTime == etime) {
				processes[i].ready = 1;
			}
		}
		qsort(processes, size, sizeof(struct process), compare);
		qsort(processes, size, sizeof(struct process), sjfc);
		while(finish < size) {
			
			if(procRunning == 0) {
				
				//qsort(processes, size, sizeof(struct process), compare);
				//qsort(processes, size, sizeof(struct process), sjfc);
				for(i = 0; i < size; i++) {
					if(processes[i].ready == 1) {
						processes[i].ready = 2;
						procRunning = 1;
						index = i;
						break;
					}
				}
			}
			if(procRunning == 0) {
				if(waitingSize > 0) {	
					int bool = 0;
					for(i = 0; i < size; i++) {
						if(processes[i].waiting == 1) {
							processes[i].recentlyAdded++;
						}
						if(processes[i].waiting == 1 && processes[i].recentlyAdded > 1) {
							bool = 1;
						}
					}
					
					if(bool == 1) {
						etime++;
					}
									
					if(verbose == 1) {
						printf("Before cycle %d: ", etime);
						for(i = 0; i < size; i++) {
							if(processes[i].cpuTime > 0) {
								if(processes[i].ready == 2) {
									printf(" running 0 " );
								}
								if(processes[i].ready == 1 && processes[i].waiting != 1 && processes[i].arrivalTime != etime) {
									printf(" ready 0 ");
								}
								if(processes[i].recentlyAdded > 1) {
									printf(" blocked %d ", processes[i].waitingTime);
								}
								if(processes[i].ready == -1 || processes[i].ready == 1 && processes[i].arrivalTime == etime) {
									printf(" unstarted 0 ");
								}
							}
							else {
								printf(" terminated 0 ");
							}
						}
						printf("\n");
					}
					
					if(waitingSize > 0) {
						for(i = 0; i < size; i++) {
							if(processes[i].waiting == 1 && processes[i].recentlyAdded > 1 && processes[i].waitingTime > 0) {
								processes[i].waitingTime--;
								processes[i].totalWaited += 1;
								if(processes[i].waitingTime == 0) {
									processes[i].waiting = 0;
									processes[i].ready = 1;
									processes[i].recentlyAdded = 0;
									waitingSize--;
								}
							}
						}
					}
				}
			}
			
			if(procRunning == 1) {
				int getBurst = randomOS(processes[index].burstTime, numFile, verbose);
				processes[index].waitingTime = getBurst * processes[index].ioTime;
				
				if(getBurst > processes[index].cpuTime) {
					processes[index].cpuTime = getBurst;
				}
				
				while(getBurst > 0) {
					etime++;
					for(i = 0; i < size; i++) {
						if(processes[i].waiting == 1) {
							processes[i].recentlyAdded++;
						}
					}
					
					if(verbose == 1) {
						printf("Before cycle %d: ", etime);
						for(i = 0; i < size; i++) {
							if(processes[i].cpuTime > 0) {
								if(processes[i].ready == 2) {
									printf(" running %d ", getBurst);
								}
								if(processes[i].ready == 1 && processes[i].waiting != 1 && processes[i].arrivalTime != etime) {
									printf(" ready 0 ");
								}
								if(processes[i].recentlyAdded > 1) {
									printf(" blocked %d ", processes[i].waitingTime);
								}
								if(processes[i].ready == -1 || processes[i].ready == 1 && processes[i].arrivalTime == etime) {
									printf(" unstarted 0 ");
								}
							}
							else {
								printf(" terminated 0 ");
							}
						}
						printf("\n");
					}
					
					for(i = 0; i < size; i++) {
						if(processes[i].ready == 1 && processes[i].waiting != 1 && processes[i].arrivalTime != etime) {
							processes[i].numRan += 1;
						}
					}
					
					getBurst--;
					processes[index].cpuTime--;
					
					if(processes[index].cpuTime > 0 && getBurst == 0) {
						waitingSize++;
						processes[index].ready = 0;
						processes[index].waiting = 1;
						processes[index].recentlyAdded = 1;
					}
					
					if(waitingSize > 0) {
						for(i = 0; i < size; i++) {
							if(processes[i].waiting == 1 && processes[i].recentlyAdded > 1) {
								processes[i].waitingTime--;
								processes[i].totalWaited += 1;
								if(processes[i].waitingTime == 0) {
									processes[i].waiting = 0;
									processes[i].ready = 1;
									processes[i].recentlyAdded = 0;
									waitingSize--;
								}
							}
						}
					}
					
					if(processes[index].cpuTime == 0) {
						finish++;
						processes[index].completed = etime;
						break;
					}
					
					for(i = 0; i < size; i++) {
						if(processes[i].arrivalTime == etime) {
							processes[i].ready = 1;
						}
					}
				}
				procRunning = 0;
			}
		}
		printf("The scheduling algorithm used was Shortest Job First\n\n");
		qsort(processes, size, sizeof(struct process), pid);
		float cpuUtil = 0;
		float ioAVG = 0;
		float turnaroundAVG = 0;
		float avgWT = 0;
		for(i = 0; i < size; i++) {
			printf("Process %d:\n", i);
			printf("          (A,B,C,M) = (%d,%d,%d,%d)\n", origProcesses[i].arrivalTime, origProcesses[i].burstTime, origProcesses[i].cpuTime, origProcesses[i].ioTime);
			printf("          Finishing time: %d\n", processes[i].completed);
			printf("          Turnaround time: %d\n", processes[i].completed - origProcesses[i].arrivalTime);
			turnaroundAVG += (processes[i].completed - origProcesses[i].arrivalTime);
			printf("          I/O time: %d\n", processes[i].totalWaited);
			printf("          Waiting time: %d\n", processes[i].numRan);
			avgWT += processes[i].numRan;
			cpuUtil += origProcesses[i].cpuTime;
			ioAVG += processes[i].totalWaited;
		}
		
		
		turnaroundAVG = turnaroundAVG / size;
		avgWT = avgWT / size;
		cpuUtil = cpuUtil / etime;
		ioAVG = ioAVG / etime;
		float throughput = size * 100.0;
		throughput = throughput / etime;
		
		printf("\n");
		printf("Summary Data:\n");
		printf("          Finishing time: %d\n", etime);
		printf("          CPU Utilization: %f\n", cpuUtil);
		printf("          I/O Utilization: %f\n", ioAVG);
		printf("          Throughput: %f processes per hundred cycles\n", throughput);
		printf("          Average turnaround time: %f\n", turnaroundAVG);
		printf("          Average waiting time: %f\n", avgWT);
		
		fclose(numFile);

	}
	else {
		printf("File %s could not be opened\n", fileName);
		exit(0);
	}
}

void fcfs(struct process processes[], char * fileName, int size, int verbose, struct process origProcesses[]) 
{
	
	//Verbose = 1 then print detailed!
	FILE *numFile;
	numFile = fopen(fileName, "r");
	if(numFile != NULL) {
		//Populate ready list with initial processes
		int etime = 0;
		int finish = 0;
		int procRunning = 0;
		int loop = 0;
		int burstPeriod = 0;
		int l;
		int waitingSize = 0;
		int waitedArray[size];
		struct process currProc;
		//Create queue
		create();
		//Add processes to ready queue
		int k;
		for(k = 0; k < size; k++) 
		{
			if(processes[k].arrivalTime == etime) 
			{
				processes[k].ready = 1;
				waitedArray[k] = 0;
				enqueue(processes[k]);
			}
		}
		while(finish < size) 
		{
			//Check if a process is running and if the queue is not empty
			if(procRunning == 0 && empty() != 0) 
			{
				currProc = frontElement();
				currProc.ready = 2;
				procRunning = 1;
				dequeue();
			}
			
			//No process running and Queue empty -> Run them anyways
			if(procRunning == 0 && empty() == 0)
			{
				int bool = 0;
				if(waitingSize > 0) {
					for(l = 0; l < size; l++) 
					{
						if(processes[l].waiting == 1) 
						{
							processes[l].recentlyAdded++;
						}
						if(processes[l].waiting == 1 && processes[l].recentlyAdded > 1) {
							bool = 1;
						}
					}
				}
				
				if(bool == 1) {
					etime++;
				}
				
				if(verbose == 1) {
					printf("Before cycle %d: ", etime);
					for(l = 0; l < size; l++) {
						if(processes[l].cpuTime > 0) {
							if(processes[l].ready == 2) {
								printf(" running 1 ");
							}
							if(processes[l].ready == 1 && processes[l].waiting != 1 && processes[l].arrivalTime != etime) {
								printf(" ready 0 ");
							}
							if(processes[l].recentlyAdded > 1) {
								printf(" blocked %d ", processes[l].waitingTime);
							}
							if(processes[l].ready == -1) {
								printf(" unstarted 0 ");
							}
						}
						else {
							printf(" terminated 0 ");
						}
					}
					printf("\n");
				}
					
				for(l = 0; l < size; l++) {
					if(processes[l].waiting == 1 && processes[l].recentlyAdded > 1) {
						processes[l].waitingTime--;
						processes[l].waited++;
						if(processes[l].waitingTime == 0) {
							processes[l].waiting = 0;
							processes[l].ready = 1;
							processes[l].recentlyAdded = 0;
							waitingSize--;
							enqueue(processes[l]);
						}
					}
				}
				
				for(k = 0; k < size; k++) 
					{
						if(processes[k].arrivalTime == etime) 
						{
							processes[k].ready = 1;
							waitedArray[k] = 0;
							enqueue(processes[k]);
						}
					}
				procRunning = 0;
			}
			
			if(procRunning == 1) 
			{
				//Get burst time
				int getBurst = randomOS(currProc.burstTime, numFile, verbose);
				currProc.waitingTime = getBurst * currProc.ioTime;
				
				if(getBurst > currProc.cpuTime)
				{
					currProc.cpuTime = getBurst;
				}
				//Deal with burst first
				processes[currProc.pid] = currProc;
				burstPeriod = 0;
				while(getBurst > 0 && currProc.cpuTime != 0) 
				{
					etime++;
					
					for(l = 0; l < size; l++) 
					{
						if(processes[l].waiting == 1) 
						{
							processes[l].recentlyAdded++;
						}
						if(processes[l].ready == 1) {
							waitedArray[l] += 1;
						}
					}
					
					if(verbose == 1) {
						printf("Before cycle %d: ", etime);
						for(l = 0; l < size; l++) {
							if(processes[l].cpuTime > 0) {
								if(processes[l].ready == 2) {
									printf(" running %d ", getBurst);
								}
								if(processes[l].ready == 1 && processes[l].waiting != 1 && processes[l].arrivalTime != etime) {
									printf(" ready 0 ");
								}
								if(processes[l].recentlyAdded > 1) {
									printf(" blocked %d ", processes[l].waitingTime);
								}
								if(processes[l].ready == -1 || processes[l].arrivalTime == etime) {
									printf(" unstarted 0 ");
								}
							}
							else {
								printf(" terminated 0 ");
							}
						}
						printf("\n");
					}
					burstPeriod++;
					currProc.cpuTime--;
					getBurst--;
					
					if(currProc.cpuTime > 0 && getBurst == 0) {
						//Put on waiting list
						waitingSize++;
						currProc.waiting = 1;
						currProc.recentlyAdded = 1;
						currProc.ready = 0;
						processes[currProc.pid] = currProc;
					}
					
					//Deal with waiting period;
					if(waitingSize > 0) {
						for(l = 0; l < size; l++) {
							if(processes[l].waiting == 1 && processes[l].recentlyAdded > 1) {
								processes[l].waitingTime--;
								processes[l].waited++;
								if(processes[l].waitingTime == 0) {
									processes[l].waiting = 0;
									processes[l].ready = 1;
									processes[l].recentlyAdded = 0;
									waitingSize--;
									enqueue(processes[l]);
								}
							}
						}
					}
					
					if(currProc.cpuTime == 0) {
						finish++;
						currProc.completed = etime;
						processes[currProc.pid] = currProc;
						break;
					}
					for(k = 0; k < size; k++) 
					{
						if(processes[k].arrivalTime == etime) 
						{
							processes[k].ready = 1;
							waitedArray[k] = 0;
							enqueue(processes[k]);
						}
					}
				}
				
				procRunning = 0;
			}
		}
		printf("The scheduling algorithm used was First Come First Served\n");
		printf("\n");
		float turnaroundAVG = 0;
		float avgWT = 0;
		float ioAVG = 0;
		float cpuUtil = 0;
		for(l = 0; l < size; l++) {
			printf("Process %d:\n", l);
			printf("          (A,B,C,M) = (%d,%d,%d,%d)\n", origProcesses[l].arrivalTime, origProcesses[l].burstTime, origProcesses[l].cpuTime, origProcesses[l].ioTime);
			printf("          Finishing time: %d\n", processes[l].completed);
			int turnaroundTime = processes[l].completed - origProcesses[l].arrivalTime;
			printf("          Turnaround time: %d\n", turnaroundTime);
			turnaroundAVG += turnaroundTime;
			printf("          I/O time: %d\n", processes[l].waited);
			ioAVG += processes[l].waited;
			printf("          Waiting time: %d\n", waitedArray[l]);
			avgWT += waitedArray[l];
			printf("\n");
			cpuUtil += origProcesses[l].cpuTime;
		}
		turnaroundAVG = turnaroundAVG / size;
		avgWT = avgWT / size;
		float throughput = size * 100.0;
		int finTime = etime - 1;
		throughput = throughput / etime;
		
		ioAVG = ioAVG / etime;
		cpuUtil = cpuUtil / etime;
		
		printf("Summary Data: \n");
		printf("          Finishing time: %d\n", etime);
		printf("          CPU Utilization: %f\n", cpuUtil);
		printf("          I/O Utilization: %f\n", ioAVG);
		printf("          Throughput: %f processes per hundred cycles\n", throughput);
		printf("          Average turnaround time: %f\n", turnaroundAVG);
		printf("          Average waiting time: %f\n", avgWT);
		
		fclose(numFile);
		
	}
	else {
		printf("File %s could not be opened\n", fileName);
		exit(0);
	}
}

void uniProgramming(struct process processes[], char * fileName, int size, int verbose, struct process origProcesses[])
{
	//Verbose = 1 then print detailed!
	FILE *numFile;
	numFile = fopen(fileName, "r");
	if(numFile != NULL) {
		int finishTime[size];
		int ioTimes[size];
		int i;
		int cycle = 0;
		for(i = 0; i < size; i++) {
			int iotimes = 0;
			while(processes[i].cpuTime > 0) {
				int getBurst = randomOS(processes[i].burstTime, numFile, verbose);
				//printf("Burst Time: %d", getBurst);
				int ioTime = getBurst * processes[i].ioTime;
				
				if(getBurst > processes[i].cpuTime) {
					processes[i].cpuTime = getBurst;
				}
				
				while(getBurst > 0) {
					processes[i].cpuTime = processes[i].cpuTime - 1;
					getBurst--;
					//Print
					cycle++;
					if(verbose == 1) {
						printf("Before cycle %d: ", cycle);
						int k;
						for(k = 0; k < size; k++) {
							if(k < i) {
								printf("terminated 0 ");
							}
							if(k == i) {
								printf("running 1 ");
							}
							if(k > i && k < size) {
								printf("ready 0 ");
							}
						}
						printf("\n");
					}
					
					if(processes[i].cpuTime == 0) {
						finishTime[i] = cycle;
						ioTimes[i] = iotimes;
						break;
					}	
				}
				
				//Waiting
				while(ioTime > 0 && processes[i].cpuTime > 0) {
					ioTime--;
					iotimes++;
					
					cycle++;
					if(verbose == 1) {
						printf("Before cycle %d: ", cycle);
						int k;
						for(k = 0; k < size; k++) {
							if(k < i) {
								printf("terminated 0 ");
							}
							if(k == i) {
								printf("blocked 1 ");
							}
							if(k > i && k < size) {
								printf("ready 0 ");
							}
						}
						printf("\n");
					}
				}
			}
		}
		
		printf("The scheduling algorithm used was Uniprocessing\n\n");
		
		//Printing 
		float ioAVG;
		float turnaroundavg = 0;
		float waitingavg = 0;
		int k;
		for(k = 0; k < size; k++){
			printf("Process %d:\n", k);
			printf("          (A,B,C,M) = (%d,%d,%d,%d)\n", origProcesses[k].arrivalTime, origProcesses[k].burstTime, origProcesses[k].cpuTime, origProcesses[k].ioTime);
			printf("          Finishing time: %d\n", finishTime[k]);
			int turnaround = finishTime[k] - origProcesses[k].arrivalTime;
			printf("          Turnaround time: %d\n", turnaround);
			turnaroundavg += turnaround;
			printf("          I/O time: %d\n", ioTimes[k]);
			ioAVG += ioTimes[k];
			if(k == 0) {
				printf("          Waiting time: 0\n");	
			}
			if(k > 0) {
				int val = k - 1;
				int waitingTime = finishTime[val] - origProcesses[k].arrivalTime;
				printf("          Waiting time: %d\n", waitingTime);

				waitingavg += waitingTime;		
			}
		}
		
		float waitingAVG = waitingavg / size;
		float turnaroundAVG = turnaroundavg	/ size;
		
		ioAVG = ioAVG / finishTime[size - 1];
		float cpuUtil = 1 - ioAVG;
		
		float throughput = size * 100.0;
		throughput = throughput / finishTime[size - 1];
		
		printf("\nSummary Data: \n");
		printf("          Finishing time: %d\n", finishTime[size -1]);
		printf("          CPU Utilization: %f\n", cpuUtil);
		printf("          I/O Utilization: %f\n", ioAVG);
		printf("          Throughput: %f processes per hundred cycles\n", throughput);
		printf("          Average turnaround time: %f\n", turnaroundAVG);
		printf("          Average waiting time: %f\n", waitingAVG);
		
		fclose(numFile);
	}
	else {
		printf("File %s could not be opened\n", fileName);
		exit(0);
	}
	
	
}

int main(int argc, char*argv[]) 
{
	if(argc >= 2) {
		//Valid number of arguments
		FILE *file;
		char * fileName;
		if(argc == 2) {
			fileName = argv[1];
		}
		if(argc == 3) {
			fileName = argv[2];
		}
		file = fopen(fileName, "r");
		char oneword[1024];
		char c;
		
		if(file != NULL) 
		{
			int numProcess;
			//Get number of processes
			c = fscanf(file, "%s", oneword);
			sscanf(oneword, "%d", &numProcess);
			
			sscanf(oneword, "%d", &numProcess);
			//List to store processes
			struct process processes[numProcess];
			
			//Create processes 
			int i;
			for(i = 0; i < numProcess; i++) 
			{
				int value;
				//Get Arrival Time
				c = fscanf(file, "%s", oneword);
				memmove(oneword, oneword + 1, strlen(oneword + 1) + 1);
				value = atoi(oneword);
				processes[i].arrivalTime = value;
				
				//Get Arrival Time
				c = fscanf(file, "%s", oneword);
				value = atoi(oneword);
				processes[i].burstTime = value;
				
				//Get Arrival Time
				c = fscanf(file, "%s", oneword);
				value = atoi(oneword);
				processes[i].cpuTime = value;
				
				//Get Arrival Time
				c = fscanf(file, "%s", oneword);
				oneword[strlen(oneword) - 1] = '\0';
				value = atoi(oneword);
				processes[i].ioTime = value;
				
				//Set running/waiting bool
				processes[i].ready = -1;
				processes[i].waitingTime = 0;
				processes[i].waiting = 0;
				processes[i].pid = i;
				processes[i].terminated = 0;
				processes[i].recentlyAdded = 0;
				processes[i].completed = 0;
				processes[i].waited = 0;
				processes[i].totalWaited = 0;
				processes[i].numRan = 0;
				processes[i].burstVal = 0;
			}
			//Need duplicate array of original values for printing
			struct process origProcesses[numProcess];
			struct process uniProcesses[numProcess];
			struct process fcfsProcesses[numProcess];
			struct process sjfProcesses[numProcess];
			struct process rrProcesses[numProcess];
			for(i = 0; i < numProcess; i++) {
				processes[i].priority = processes[i].arrivalTime;
				origProcesses[i] = processes[i];
				uniProcesses[i] = processes[i];
				fcfsProcesses[i] = processes[i];
				sjfProcesses[i] = processes[i];
				rrProcesses[i] = processes[i];
			}
			
			int verbose = 0;
			if(argc == 3) {
				if(strcmp(argv[1], "--verbose") == 0) {
					verbose = 1;
				}
			}
			char * randFileName = "random-numbers";
			
			//Running algorithms
			
			//Print original input
			printf("The original input was: %d ", numProcess);
			for(i = 0; i < numProcess; i++) {
				printf("( %d %d %d %d ) ", processes[i].arrivalTime, processes[i].burstTime, processes[i].cpuTime, processes[i].ioTime); 
			}
			qsort(processes, numProcess, sizeof(struct process), compare);
			printf("\n");
			printf("The (sorted) input is:  %d ", numProcess);
			for(i = 0; i < numProcess; i++) {
				printf("( %d %d %d %d ) ", processes[i].arrivalTime, processes[i].burstTime, processes[i].cpuTime, processes[i].ioTime); 
			}
			printf("\n\n");
			if(verbose == 1) {
				printf("This detailed printout gives the state and remaining burst for each process\n\n");
				printf("Before cycle 0: ");
				for(i = 0; i < numProcess; i++) 
				{
					printf("unstarted 0 ");
				}
				printf("\n");
			}
			fcfs(fcfsProcesses, randFileName, numProcess, verbose, origProcesses);
			printf("\n");
			
			
			printf("The original input was: %d ", numProcess);
			for(i = 0; i < numProcess; i++) {
				printf("( %d %d %d %d ) ", origProcesses[i].arrivalTime, origProcesses[i].burstTime, origProcesses[i].cpuTime, origProcesses[i].ioTime); 
			}
			printf("\n");
			printf("The (sorted) input is:  %d ", numProcess);
			for(i = 0; i < numProcess; i++) {
				printf("( %d %d %d %d ) ", processes[i].arrivalTime, processes[i].burstTime, processes[i].cpuTime, processes[i].ioTime); 
			}
			printf("\n\n");
			if(verbose == 1) {
				printf("This detailed printout gives the state and remaining burst for each process\n\n");
				printf("Before cycle 0: ");
				for(i = 0; i < numProcess; i++) 
				{
					printf("unstarted 0 ");
				}
				printf("\n");
			}
			qsort(rrProcesses, numProcess, sizeof(struct process), compare);
			rr(rrProcesses, randFileName, numProcess, verbose, origProcesses);
			printf("\n");
			
			
			printf("The original input was: %d ", numProcess);
			for(i = 0; i < numProcess; i++) {
				printf("( %d %d %d %d ) ", origProcesses[i].arrivalTime, origProcesses[i].burstTime, origProcesses[i].cpuTime, origProcesses[i].ioTime); 
			}
			printf("\n");
			printf("The (sorted) input is:  %d ", numProcess);
			for(i = 0; i < numProcess; i++) {
				printf("( %d %d %d %d ) ", processes[i].arrivalTime, processes[i].burstTime, processes[i].cpuTime, processes[i].ioTime); 
			}
			printf("\n\n");
						if(verbose == 1) {
				printf("This detailed printout gives the state and remaining burst for each process\n\n");
				printf("Before cycle 0: ");
				for(i = 0; i < numProcess; i++) 
				{
					printf("unstarted 0 ");
				}
				printf("\n");
			}
			uniProgramming(uniProcesses, randFileName, numProcess, verbose, origProcesses);
			
			printf("\n");
			printf("The original input was: %d ", numProcess);
			for(i = 0; i < numProcess; i++) {
				printf("( %d %d %d %d ) ", origProcesses[i].arrivalTime, origProcesses[i].burstTime, origProcesses[i].cpuTime, origProcesses[i].ioTime); 
			}
			printf("\n");
			sjf(sjfProcesses, randFileName, numProcess, verbose, origProcesses);
			
		}
		else 
		{
			printf("Could not open input %s file.\n", fileName);
			exit(0);
		}
		
		fclose(file);
	}
	else {
		printf("Not enough command line arguments.<Optional --verbose> <File name> \n");
	}
	return 0;
}

