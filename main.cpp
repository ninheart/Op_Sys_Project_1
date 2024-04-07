#include <iostream>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <vector>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <cmath>
#include <algorithm>
#include "Process.h"
#include "CPU.h"
using namespace std;

char alphabets[] = {'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z'};

// easy way to pass the type of scheduler to the simulation function
enum scheduler
{
	fcfs,
	sjf,
	srt,
	rr
};

// next_exp function for generating random values
double next_exp(double lambda, int upperBound)
{
	double x;
	do
	{
		double r = drand48(); // Uniform dist [0.00,1.00)
		x = -log(r) / lambda; // Convert to exponential distribution
	} while (x > upperBound); // Skip values above the upperBound
	return x;
}

// resets process parameters for next simulation
void reset(vector<Process> &procceses, int n)
{
	for (int i = 0; i < n; ++i)
	{
		procceses[i].reset();
	}
}
// prints the queue in the specified format
void printQueue(std::vector<Process> &queue)
{
	std::cout << "[Q";
	if (queue.size() == 0)
	{
		std::cout << " <empty>]" << std::endl;
		return;
	}
	for (uint i = 0; i < queue.size(); i++)
	{
		std::cout << " " << queue[i].id;
	}
	std::cout << "]" << std::endl;
}

// compare two processes by least arrival time (and if they are equal, by least ID)
bool compareArrivalTime(const Process &p1, const Process &p2)
{
	if (p1.arrivalTime != p2.arrivalTime)
	{
		// If arrival times are different, sort based on arrival time
		return p1.arrivalTime < p2.arrivalTime;
	}
	else
	{
		// If arrival times are the same, sort based on ID
		return p1.id < p2.id;
	}
}

// compare two processes by least tau value (and if they are equal, by CPU time and then by least ID)
bool compareTau(const Process &p1, const Process &p2)
{
	// Compare tau if not equal, otherwise the id
	if(p1.tau != p2.tau)
		return p1.tau < p2.tau;
	else if(p1.cpuTime != p2.cpuTime)
		return p1.cpuTime > p2.cpuTime;
	else
		return p1.id < p2.id;
}

// compare two processes by least lexicographical order
bool compareID(const Process &p1, const Process &p2)
{
	return p1.id < p2.id;
}

string ceilTo3(double value){
	double roundedValue = std::ceil(value * 1000.0) / 1000.0;
	stringstream ss;
	ss << std::fixed << std::setprecision(3) << roundedValue;
	return ss.str();
}

// calculates the average CPU utilization for a group of processes and returns it in the specified format as a String
string cpuUtilization(int time, vector<Process> processes, int n){
	double totalBurstTime = 0;
	for(int i = 0; i < n; ++i){
		for(uint j = 0; j < processes[i].cpuBurstTime.size(); ++j){
			totalBurstTime += processes[i].cpuBurstTime[j];
		}
	}
	return ceilTo3(totalBurstTime / time * 100);
}

// calculates the average CPU burst time for a group of processes and returns it in the specified format as a String
string avgCpuBurstTime(int time, vector<Process> processes, int n, int num_cpu)
{
	double avgBurst = 0;
	int burstCount = 0;

	double avgIOBoundBurst = 0;
	double avgCPUBoundBurst = 0;
	int cpuBurst = 0;
	int ioBurst = 0;
	for(int i = 0; i < n; i++)
	{
		for(uint j = 0; j < processes[i].cpuBurstTime.size(); j++)
		{
			if(processes[i].cpuBound)
			{
				avgCPUBoundBurst += processes[i].cpuBurstTime[j];
				cpuBurst++;
			}
			else
			{
				avgIOBoundBurst += processes[i].cpuBurstTime[j];
				ioBurst++;
			}
			avgBurst += processes[i].cpuBurstTime[j];
			burstCount++;
		}
	}
	return ceilTo3(avgBurst / burstCount) + " ms (" + ceilTo3(avgCPUBoundBurst / cpuBurst) + " ms/" + ceilTo3(avgIOBoundBurst / ioBurst) + " ms)";
}

// calculates the average process wait time for a group of processes and returns it in the specified format as a String
string avgWaitTime(int time, vector<Process> processes, int n, int num_cpu)
{
	// for ease of coding, start with the burst code from above
	double avgBurst = 0;
	double avgIOBoundBurst = 0;
	double avgCPUBoundBurst = 0;

	int burst = 0;
	int ioBurst = 0;
	int cpuBurst = 0;
	for(int i = 0; i < n; i++)
	{
		for(uint j = 0; j < processes[i].waitTimes.size(); j++)
		{
			if(processes[i].cpuBound)
			{
				avgCPUBoundBurst += processes[i].waitTimes[j];
				cpuBurst++;
			}
			else
			{
				avgIOBoundBurst += processes[i].waitTimes[j];
				ioBurst++;
			}
			avgBurst += processes[i].waitTimes[j];
			burst++;
		}
	}
	return ceilTo3(avgBurst / burst) + " ms (" + ceilTo3(avgCPUBoundBurst / cpuBurst) + " ms/" +ceilTo3(avgIOBoundBurst / ioBurst) +" ms)";
}

// calculates the average process turnaround time for a group of processes and reutrns it in the specified format as a String
string avgTurnaround(int time, vector<Process>& processes, int n, int num_cpu, int cpuCS, int ioCS, double t_cs)
{
	// Turnaround formula
	// = 1/2 in context switch + 1/2 out context switch + wait time + burst time
	// = # of switches * t_cs + wait time + burst time
	//
	// for ease of coding, start with the burst code from above
	double avgBurst = 0;
	double avgIOBoundBurst = 0;
	double avgCPUBoundBurst = 0;

	int burst = 0;
	int ioBurst = 0;
	int cpuBurst = 0;

	// Add wait time and burst time
	for (int i = 0; i < n; i++)
	{
		for(uint j = 0; j < processes[i].waitTimes.size(); j++)
		{
			if (j < processes[i].cpuBurstTime.size())
			{

				if (processes[i].cpuBound)
				{
					avgCPUBoundBurst += processes[i].waitTimes[j];
					avgCPUBoundBurst += processes[i].cpuBurstTime[j];
					cpuBurst++;
				}
				else
				{
					avgIOBoundBurst += processes[i].waitTimes[j];
					avgIOBoundBurst += processes[i].cpuBurstTime[j];
					ioBurst++;
				}

				avgBurst += processes[i].waitTimes[j];
				avgBurst += processes[i].cpuBurstTime[j];
				burst++;
				
			}
		}
	}


	// Add in # of switches
	avgCPUBoundBurst += cpuCS * t_cs;
	avgIOBoundBurst += ioCS * t_cs;
	avgBurst += (ioCS + cpuCS) * t_cs;

	return ceilTo3(avgBurst / burst) + " ms (" + ceilTo3(avgCPUBoundBurst / cpuBurst) + " ms/" + ceilTo3(avgIOBoundBurst / ioBurst) + " ms)";
}

void FCFS(vector<Process> &processes, int n, int t_cs, int num_cpu, ofstream &outputFile)
{
	sort(processes.begin(), processes.end(), compareArrivalTime);

	int time = 0;
	int alive = n;
	int contextSwitch = 0;
	int cpuContextSwitch = 0;
	int ioContextSwitch = 0;

	CPU cpu;
	cpu.context = 0;
	cout << "time " << time << "ms:"
		 << " Simulator started for FCFS ";
	cpu.printQueue();

	while (alive > 0)
	{

		if (cpu.context > 0)
			cpu.context--;

		if (cpu.context == 0 && cpu.switchingProcess != NULL)
		{
			Process *temp = cpu.switchingProcess;
			if (temp->swap)
			{
				temp->inCPU = true;
				cpu.currentProcess = temp;
				cpu.switchingProcess = NULL;
				cpu.popFront();

				if (time < 10000)
				{
					cout << "time " << time << "ms: ";
					cout << "Process " << temp->id << " started using the CPU for " << temp->cpuBurstTime[temp->step] << "ms burst ";
					cpu.printQueue();
				}

				contextSwitch +=1;

				if (temp->cpuBound)
					cpuContextSwitch++;
				else
					ioContextSwitch++;
			}
			else
			{
				temp->inIO = true;
				cpu.switchingProcess = NULL;
				temp->turn = false;
			}
		}

		for (int i = 0; i < n; i++)
		{
			Process *p = &(processes[i]);
			// add process to queue

			if (time == p->nextArrivalTime)
			{
				cpu.addProcess(*p);
				p->inQueue = true;
				p->turn = true;

				if (p->inIO)
				{
					p->inIO = false;
					p->step++;
					if (time < 10000)
					{
						cout << "time " << time << "ms: ";
						cout << "Process " << p->id << " completed I/O; added to ready queue ";
						cpu.printQueue();
					}

				}else{
					if (time < 10000)
					{
						cout << "time " << time << "ms: ";
						cout << "Process " << p->id << " arrived; added to ready queue ";
						cpu.printQueue();
					}
				}
			}

			if (p->inQueue)
			{
				// if there's no other process runnning then run this process
				if (cpu.currentProcess == NULL && cpu.switchingProcess == NULL && *p == cpu.front())
				{
					p->inQueue = false;
					p->swap = true;
					// waitTime += p->waitTime;
					p->waitTimes.push_back(p->waitTime);
					// count++;
					cpu.context += t_cs / 2;
					cpu.switchingProcess = p;
					p->cpuTime = 0;
					p->waitTime = 0;
				}
				else
				{
					p->waitTime += 1;
				}
			}


			if(p->inCPU){
				// now check if the burst is done
				if(p->cpuTime == p->cpuBurstTime[p->step]){

					// turnaroundTime += time - p->nextArrivalTime + t_cs/2; //turnaround time 
					// cpu.currentProcess = NULL;
					cpu.switchingProcess = p;
					cpu.context += t_cs/2;
					p->swap = false;
					p->inCPU = false;

					if(p->step == int(p->cpuBurstTime.size()-1)){
						p->inQueue = false;
						p->inIO = false;

						alive--;
						cout<<"time "<<time<<"ms: ";
						cout<<"Process "<<p->id<<" terminated ";
						cpu.printQueue();
						cpu.currentProcess = NULL;
						continue;
					}

					int updateArrivalTime=time+(p->ioBurstTime)[p->step]+(t_cs/2);
					p->nextArrivalTime = updateArrivalTime;

					if(time < 10000){
						cout << "time " << time << "ms: Process " << p->id << " completed a CPU burst; " << p->cpuBurstTime.size() - p->step - 1;
						if (p->cpuBurstTime.size() - p->step - 1 == 1)
						{
							cout << " burst to go ";
						}
						else
						{
							cout << " bursts to go ";
						}

						cpu.printQueue();
					}

					if (time < 10000)
					{
						cout << "time " << time << "ms: Process " << p->id << " switching out of CPU; blocking on I/O until time " << updateArrivalTime << "ms ";
						cpu.printQueue();
					}
					// cout << "time " << time << "ms: Process " << p->id << " switching out of CPU; blocking on I/O until time " << updateArrivalTime<<"ms ";
					// cpu.printQueue();

					// switch out process
					cpu.currentProcess = NULL;
				}
				p->cpuTime++;
			}
		}
		time++;
	}

	time = time + t_cs/2 -1;
	cout << "time " << time << "ms: Simulator ended for FCFS ";
	cpu.printQueue();


	outputFile << "Algorithm FCFS" << endl;
	outputFile <<"-- CPU utilization: "<<cpuUtilization(time,processes,n)<<"%"<<endl;
	outputFile << "-- average CPU burst time: " << avgCpuBurstTime(time, processes, n, num_cpu) << endl;
	outputFile << "-- average wait time: " << avgWaitTime(time, processes, n, num_cpu) << endl;
	outputFile << "-- average turnaround time: " << avgTurnaround(time, processes, n, num_cpu, cpuContextSwitch, ioContextSwitch, t_cs) << endl;
	outputFile << "-- number of context switches: " << contextSwitch << " (" << cpuContextSwitch << "/" << ioContextSwitch << ")" << std::endl;
	;
	outputFile << "-- number of preemptions: 0 (0/0)"<<endl;
	outputFile << endl;
}

// SJF algorithm
void SJF(vector<Process> &processes, int n, int t_cs, double alpha, int num_cpu, ofstream &outputFile)
{
	sort(processes.begin(), processes.end(), compareArrivalTime);

	int time = 0;
	int alive = n;
	int contextSwitch = 0;
	int cpuContextSwitch = 0;
	int ioContextSwitch = 0;

	CPU cpu;
	cpu.context = 0;
	cout << "time " << time << "ms:" << " Simulator started for SJF ";
	cpu.printQueue();

	while (alive > 0)
	{
		if (cpu.context > 0)
			cpu.context--;

		if (cpu.context == 0 && cpu.switchingProcess != NULL)
		{
			Process *temp = cpu.switchingProcess;
			if (temp->swap)
			{
				temp->inCPU = true;
				cpu.currentProcess = temp;
				cpu.switchingProcess = NULL;
				temp->turnaroundTime += t_cs / 2 + 1;

				// Count context switches
				contextSwitch++;
				if(temp->cpuBound)
					cpuContextSwitch++;
				else
					ioContextSwitch++;

				if(time < 10000)
				{
					cout<<"time "<<time<<"ms: " << "Process " << temp->id << " (tau " << temp->tau << "ms) started using the CPU for " << temp->cpuBurstTime[temp->step] <<"ms burst ";
					cpu.printQueue();
				}
			}
			else
			{
				temp->inIO = true;
				cpu.switchingProcess = NULL;
				temp->turn = false;
			}
		}

		for (int i = 0; i < n; i++)
		{
			Process *p = &(processes[i]);

			// add process to queue
			if (time == p->nextArrivalTime)
			{
				// add the process and sort it according to tau
				cpu.addProcess(*p);
				deque<Process>* q = cpu.getProcessQueue();
				std::sort(q->begin(), q->end(), compareTau);
				p->beginTime = time;
				p->inQueue = true;

				if(p->inIO)
				{
					p->inIO = false;
					p->step++;
					if(time < 10000)
					{
						cout << "time " << time << "ms: " <<"Process "<< p->id <<" (tau " << p->tau << "ms) completed I/O; added to ready queue ";
						cpu.printQueue();
					}
				}
				else
				{
					if(time < 10000)
					{
						cout << "time " << time << "ms: " << "Process " << p->id << " (tau " << p->tau << "ms) arrived; added to ready queue ";
						cpu.printQueue();
					}
				}
			}

			// Check queue status (if it is time to switch)
			if (p->inQueue)
			{
				// if there's no other process runnning then run this process
				if (cpu.currentProcess == NULL && cpu.switchingProcess == NULL && *p == cpu.front())
				{
					p->inQueue = false;
					p->swap = true;
					cpu.context += t_cs / 2;
					cpu.switchingProcess = p;
					p->cpuTime = 0;
					p->waitTimes.push_back(p->waitTime);
					p->waitTime = 0;
					cpu.popFront();
				}
				else
				{
					p->waitTime += 1;
				}
			}

			// Check CPU status (if the burst is done)
			if(p->inCPU)
			{
				// if it is time to burst
				if(p->cpuTime == p->cpuBurstTime[p->step])
				{
					cpu.switchingProcess = p;
					cpu.context += t_cs/2;
					p->swap = false;
					p->inCPU = false;

					if(p->step == int(p->cpuBurstTime.size()-1)){
						p->inQueue = false;
						p->inIO = false;

						alive--;
						cout<<"time "<<time<<"ms: ";
						cout<<"Process "<<p->id<<" terminated ";
						cpu.printQueue();

						cpu.currentProcess = NULL;
						continue;
					}

					// Burst completion messaging
					int updateArrivalTime = time+(p->ioBurstTime)[p->step]+(t_cs/2);
					p->nextArrivalTime = updateArrivalTime;

					if(time < 10000)
						cout<<"time "<<time<<"ms: Process "<<p->id<<" (tau " << p->tau << "ms) completed a CPU burst; "<<p->cpuBurstTime.size()-p->step-1;

					// Print whether there is a single or multiple bursts left
					if(p->cpuBurstTime.size() - p->step - 1 == 1 && time < 10000)
						cout << " burst to go ";
					else if(time < 10000)
						cout << " bursts to go ";
					if(time < 10000)
						cpu.printQueue();

					// Recalculate tau once the burst is done
					int newTau = ceil((1-alpha)*p->tau + (alpha)*p->cpuBurstTime[p->step]);
					if(time < 10000)
					{
						std::cout << "time " << time << "ms: Recalculating tau for process " << p->id << ": old tau " << p->tau << "ms ==> new tau " << newTau << "ms ";
						cpu.printQueue();
					}
					p->tau = newTau; 

					if(time < 10000)
					{
						cout << "time " << time << "ms: Process " << p->id << " switching out of CPU; blocking on I/O until time " << updateArrivalTime << "ms ";
						cpu.printQueue();
					}

					// Switch out the process
					cpu.currentProcess = NULL;
				}
				p->cpuTime++;
			}
		}
		time++;
	}
	time = time + t_cs/2 -1;
	cout << "time " << time << "ms: Simulator ended for SJF ";
	cpu.printQueue();

	outputFile << "Algorithm SJF" << std::endl;
	outputFile << "-- CPU utilization: " << cpuUtilization(time, processes, n) << "%" << std::endl;
	outputFile << "-- average CPU burst time: " << avgCpuBurstTime(time, processes, n, num_cpu) << std::endl;
	outputFile << "-- average wait time: " << avgWaitTime(time, processes, n, num_cpu) << std::endl;
	outputFile << "-- average turnaround time: " << avgTurnaround(time, processes, n, num_cpu, cpuContextSwitch, ioContextSwitch, t_cs) << std::endl;
	outputFile << "-- number of context switches: " << contextSwitch << " (" << cpuContextSwitch << "/" << ioContextSwitch << ")" << std::endl;
	outputFile << "-- number of preemptions: 0 (0/0)" << std::endl;
	outputFile << endl;
}

// SRT algorithm
void SRT(vector<Process> &processes, int n, int t_cs, double alpha, int num_cpu, ofstream &outputFile)
{
	sort(processes.begin(), processes.end(), compareArrivalTime);

	int time = 0;
	int alive = n;
	int contextSwitch = 0;
	int cpuContextSwitch = 0;
	int ioContextSwitch = 0;
	int preemptions = 0;
	int cpuPreemptions = 0;
	int ioPreemptions = 0;

	CPU cpu;
	cpu.context = 0;
	cout << "time " << time << "ms:" << " Simulator started for SRT ";
	cpu.printQueue();

	while (alive > 0)
	{
		// context tick
		if (cpu.context > 0)
			cpu.context--;

		// cpu switching tick
		if (cpu.context == 0 && cpu.switchingProcess != NULL)
		{
			Process *temp = cpu.switchingProcess;
			if (temp->swap)
			{
				temp->swap = false;
				temp->inCPU = true;
				cpu.currentProcess = temp;
				cpu.switchingProcess = NULL;
				temp->turnaroundTime += t_cs / 2 + 1;

				// Count context switches
				contextSwitch++;
				if(temp->cpuBound)
					cpuContextSwitch++;
				else
					ioContextSwitch++;

				if(temp->remainingTime != temp->cpuBurstTime[temp->step] && (temp->remainingTime != 0 && temp->remainingTime != -1) && time < 10000)
				{
					cout << "time " << time << "ms: " << "Process " << temp->id << " (tau " << temp->tau << "ms) " << "started using the CPU for remaining " << temp->remainingTime << "ms of " << temp->cpuBurstTime[temp->step] << "ms burst ";
					cpu.printQueue();
				}
				else if(time < 10000)
				{
					cout << "time " << time << "ms: " << "Process " << temp->id << " (tau " << temp->tau << "ms) " << "started using the CPU for " << temp->cpuBurstTime[temp->step] << "ms burst ";
					cpu.printQueue();
				}
			}
			else
			{
				temp->inIO = true;
				cpu.switchingProcess = NULL;
				temp->turn = false;
			}
		}

		// process specific updates at every tick
		for (int i = 0; i < n; i++)
		{
			Process *p = &(processes[i]);
			p->processed = true;

			// add process to queue
			if(time == p->nextArrivalTime)
			{
				// add the process and sort it according to tau
				cpu.addProcess(*p);
				deque<Process>* q = cpu.getProcessQueue();
				std::sort(q->begin(), q->end(), compareTau);
				p->beginTime = time;
				p->inQueue = true;

				// preempt
				if(cpu.currentProcess != NULL && cpu.context == 0)
				{
					Process* p2 = cpu.currentProcess;
					if(p->tau < (p2->tau - p2->cpuTime))
					{
						preemptions++;
						if(p->cpuBound)
							cpuPreemptions++;
						else
							ioPreemptions++;

						Process* p2 = cpu.currentProcess;
						cpu.context += t_cs;

						if(time < 10000)
						{
							std::cout << "time " << time << "ms: Process " << p->id << " (tau " << p->tau << "ms) completed I/O; preempting " << p2->id << " ";
							cpu.printQueue();
						}
						cpu.popFront();

						cpu.addProcess(*p2);
						deque<Process>* q = cpu.getProcessQueue();
						std::sort(q->begin(), q->end(), compareTau);

						p2->inQueue = true;
						p2->beginTime = time;
						p2->inCPU = false;

						if(p2->processed)
							p2->remainingTime++;

						p->step++;

						cpu.switchingProcess = p;

						p->inQueue = false;
						p->remainingTime = p->cpuBurstTime[p->step];
						p->cpuTime = 0;
						p->waitTimes.push_back(p->waitTime);
						p->preempt = true;
						p->swap = true;

						goto queueStep;
					}
				}

				// normal add operation (completed IO or initial arrival)
				if(p->inIO)
				{
					p->inIO = false;
					p->step++;
					if(time < 10000)
					{
						cout << "time " << time << "ms: " <<"Process "<< p->id <<" (tau " << p->tau << "ms) completed I/O; added to ready queue ";
						cpu.printQueue();
					}
				}
				else
				{
					if(time < 10000)
					{
						cout << "time " << time << "ms: " << "Process " << p->id << " (tau " << p->tau << "ms) arrived; added to ready queue ";
						cpu.printQueue();
					}
				}
			}
			queueStep:

			// Check queue status (if it is time to switch)
			if (p->inQueue)
			{
				// if there's no other process runnning then run this process
				if (cpu.currentProcess == NULL && cpu.switchingProcess == NULL && *p == cpu.front())
				{
					p->inQueue = false;
					p->swap = true;
					cpu.context += t_cs / 2;
					cpu.switchingProcess = p;

					if(p->timeLeft == 0 || p->timeLeft == -1)
						p->timeLeft = p->cpuBurstTime[p->step];
					if(p->remainingTime == 0 || p->remainingTime == -1)
					{
						p->remainingTime = p->cpuBurstTime[p->step];
						p->cpuTime = 0;
					}
					p->waitTimes.push_back(p->waitTime);
					p->waitTime = 0;
					cpu.popFront();
				}
				else
				{
					p->waitTime += 1;
				}
			}

			// Check CPU status (if the burst is done)
			if(p->inCPU)
			{
				// if it is time to burst
				if(p->remainingTime == 0 || p->remainingTime == -1)
				{
					p->preempt = false;
					cpu.switchingProcess = p;
					cpu.context += t_cs/2;
					p->inCPU = false;

					if(p->step == int(p->cpuBurstTime.size()-1))
					{
						p->inQueue = false;
						p->inIO = false;

						alive--;
						cout<<"time "<<time<<"ms: ";
						cout<<"Process "<<p->id<<" terminated ";
						cpu.printQueue();

						cpu.currentProcess = NULL;
						goto cpuSkip;
					}

					// Burst completion messaging
					int updateArrivalTime = time+(p->ioBurstTime)[p->step]+(t_cs/2);
					p->nextArrivalTime = updateArrivalTime;

					if(time < 10000)
						cout<<"time "<<time<<"ms: Process "<<p->id<<" (tau " << p->tau << "ms) completed a CPU burst; "<<p->cpuBurstTime.size()-p->step-1;

					// Print whether there is a single or multiple bursts left
					if(p->cpuBurstTime.size() - p->step - 1 == 1 && time < 10000)
						cout << " burst to go ";
					else if(time < 10000)
						cout << " bursts to go ";
					if(time < 10000)
						cpu.printQueue();

					// Recalculate tau once the burst is done
					int newTau = ceil((1-alpha)*p->tau + (alpha)*p->cpuBurstTime[p->step]);
					if(time < 10000)
					{
						std::cout << "time " << time << "ms: Recalculating tau for process " << p->id << ": old tau " << p->tau << "ms ==> new tau " << newTau << "ms ";
						cpu.printQueue();
					}
					p->tau = newTau; 

					if(time < 10000|| time > 10000)
					{
						cout << "time " << time << "ms: Process " << p->id << " switching out of CPU; blocking on I/O until time " << updateArrivalTime << "ms ";
						cpu.printQueue();
					}
					p->cpuTime = 0;

					// Switch out the process
					cpu.currentProcess = NULL;
					goto cpuSkip;
				}
				p->cpuTime++;
				p->timeLeft--;
				p->remainingTime--;
			}
			cpuSkip:;
		}
		time++;

		for(int i = 0; i < (int) processes.size(); i++)
			processes[i].processed = false;
	}
	time = time + t_cs/2 -1;
	cout << "time " << time << "ms: Simulator ended for SRT ";
	cpu.printQueue();

	outputFile << "Algorithm SRT" << std::endl;
	outputFile << "-- CPU utilization: " << cpuUtilization(time, processes, n) << "%" << std::endl;
	outputFile << "-- average CPU burst time: " << avgCpuBurstTime(time, processes, n, num_cpu) << std::endl;
	outputFile << "-- average wait time: " << avgWaitTime(time, processes, n, num_cpu) << std::endl;
	outputFile << "-- average turnaround time: " << avgTurnaround(time, processes, n, num_cpu, cpuContextSwitch, ioContextSwitch, t_cs) << std::endl;
	outputFile << "-- number of context switches: " << contextSwitch << " (" << cpuContextSwitch << "/" << ioContextSwitch << ")" << std::endl;
	outputFile << "-- number of preemptions: " << preemptions << " (" << ioPreemptions << "/" << cpuPreemptions << ")" << std::endl;
	outputFile << endl;
}

void RR(vector<Process> &processes, int n, int t_cs, int t_slice, int num_cpu, ofstream &outputFile){
	sort(processes.begin(), processes.end(), compareArrivalTime);

	int time = 0;
	int alive = n;
	double waitTime = 0, count = 0;
	//double turnaroundTime = 0;
	int contextSwitch = 0;
	int cpuContextSwitch = 0;
	int ioContextSwitch = 0;
	int preemption = 0;
	int cpuPreemption = 0;
	int ioPreemption = 0;

	double cpuWaitTime = 0, ioWaitTime = 0, cpuWaitCount = 0, ioWaitCount = 0;


	CPU cpu;
	cpu.context = 0;
	cout << "time " << time << "ms:"
		 << " Simulator started for RR ";
	cpu.printQueue();

	while (alive > 0)
	{

		if (cpu.context > 0)
			cpu.context--;

		if (cpu.context == 0 && cpu.switchingProcess != NULL)
		{
			// cout<<frontProcess.preempt<<endl;
			Process *temp = cpu.switchingProcess;
			if (temp->swap)
			{
				temp->inCPU = true;
				cpu.currentProcess = temp;
				cpu.switchingProcess = NULL;

				// cpu.popFront();
				if(time < 10000){
					if (temp->remainingTime < temp->cpuBurstTime[temp->step])
					{
						cout << "time " << time << "ms: ";
						cout << "Process " << temp->id << " started using the CPU for remaining " << temp->remainingTime << "ms of " << temp->cpuBurstTime[temp->step] << "ms burst ";
					}
					else
					{
						cout << "time " << time << "ms: ";
						cout << "Process " << temp->id << " started using the CPU for " << temp->cpuBurstTime[temp->step] << "ms burst ";
					}
					cpu.printQueue();
				}
				
				contextSwitch += 1;


				if (temp->cpuBound)
					cpuContextSwitch++;
				else
					ioContextSwitch++;

			}else if(temp->preempt){
				temp->inQueue = true;
				cpu.addProcess(*temp);
				cpu.switchingProcess = NULL;
			}
			else
			{
				temp->inIO = true;
				cpu.switchingProcess = NULL;
				temp->turn = false;
			}
		}

		// reorder queue based on io burst and id
		for(int i = 0; i < n; ++i){
			Process *p = &(processes[i]);
			// vector<Process*> sameIOCompletion;
			if(time == p->nextArrivalTime && p->inIO){
				for(int j = i+1; j < n; ++j){
					Process *pNext = &(processes[j]);
					if(pNext->nextArrivalTime == p->nextArrivalTime && pNext->inIO){
						swap(processes[i],processes[j]);
					}else{
						break;
					}
				}
			}
		}


		for(int i = 0; i < n; i++){
			Process* p = &(processes[i]);

			if (time == p->nextArrivalTime && p->inIO)
			{
				cpu.addProcess(*p);
				p->inQueue = true;
				p->inIO = false;
				p->step++;
				if(time < 10000){
					cout << "time " << time << "ms: ";
					cout << "Process " << p->id << " completed I/O; added to ready queue ";
					cpu.printQueue();
				}
				p->remainingTime = p->cpuBurstTime[p->step];
				p->turn = true;
			}


			if(p->inCPU){
				if((p->cpuTime != 0 && p->cpuTime % t_slice == 0) || p->cpuTime == p->remainingTime){
					
					if(p->cpuTime == p->remainingTime || cpu.getQueueSize()!=0){
						cpu.currentProcess = NULL;
						cpu.switchingProcess = p;
						cpu.context += t_cs /2;
						p->swap = false;
						p->inCPU = false;
					}

					if(p->cpuTime == p->remainingTime){
						if (p->cpuBound)
						{
							cpuWaitCount += 1;
						}
						else
						{
							ioWaitCount += 1;
						}
						count+=1;
						p->preempt = false;

						if(p->step == int(p->cpuBurstTime.size()-1)){
							p->inQueue = false;
							p->inIO = false;
							alive--;
							cout<<"time "<<time<<"ms: Process "<<p->id<<" terminated ";
							cpu.printQueue();
							continue;
						}
						int updateArrivalTime=time+(p->ioBurstTime)[p->step]+(t_cs/2);
						p->nextArrivalTime = updateArrivalTime;

						if(time < 10000){
							cout << "time " << time << "ms: ";
							if (p->cpuBurstTime.size() - p->step - 1 == 1)
							{

								cout << "Process " << p->id << " completed a CPU burst; 1 burst to go ";
							}
							else
							{
								cout << "Process " << p->id << " completed a CPU burst; " << p->cpuBurstTime.size() - p->step - 1 << " bursts to go ";
							}
							cpu.printQueue();
							cout << "time " << time << "ms: Process " << p->id << " switching out of CPU; blocking on I/O until time " << updateArrivalTime << "ms ";
							cpu.printQueue();
						}
					}
					else
					{
						p->remainingTime -= p->cpuTime;
						p->cpuTime = 0;
						if(cpu.getQueueSize() == 0){
							p->preempt = false;
							if(time < 10000){
								cout<<"time "<<time<<"ms: Time slice expired; no preemption because ready queue is empty ";
								cpu.printQueue();
							}

						}else{
							p->preempt = true;
							if(p->cpuBound){
								cpuPreemption += 1;
							}else{
								ioPreemption += 1;
							}
							preemption++;
							if(time < 10000){
								cout<<"time "<<time<<"ms: Time slice expired; preempting process "<<p->id<<" with "<<p->remainingTime<<"ms remaining ";
								cpu.printQueue();
							}
						}
					}
				}
				p->cpuTime++;
			}
			if (time == p->nextArrivalTime && !p->inQueue && !p->inIO && !p->inCPU)
			{
				cpu.addProcess(*p);
				p->inQueue = true;
				if (time < 10000)
				{
					cout << "time " << time << "ms: ";
					cout << "Process " << p->id << " arrived; added to ready queue ";
					cpu.printQueue();
				}
				p->remainingTime = p->cpuBurstTime[p->step];
				p->turn = true;
			}

			if (p->inQueue)
			{
				if (cpu.currentProcess == NULL && cpu.switchingProcess == NULL && *p == cpu.front())
				{
					p->inQueue = false;
					p->swap = true;
					cpu.switchingProcess = p;
					cpu.context += t_cs / 2;
					p->cpuTime = 0;
					cpu.popFront();
					if (p->cpuBound)
					{
						cpuWaitTime += p->waitTime;
					}
					else
					{
						ioWaitTime += p->waitTime;
					}

					p->waitTimes.push_back(p->waitTime);
					waitTime += p->waitTime;
					p->waitTime = 0;
				}
				else
				{
					p->waitTime += 1;
				}
			}
		}
		time++;
	}


	time = time + t_cs / 2 - 1;
	cout << "time " << time << "ms: Simulator ended for RR ";
	cpu.printQueue();


	// cout << t_cs << endl;

	outputFile << "Algorithm RR" << endl;
	outputFile << "-- CPU utilization: " << cpuUtilization(time, processes, n) << "%" << endl;
	outputFile << "-- average CPU burst time: " << avgCpuBurstTime(time, processes, n, num_cpu) << endl;
	outputFile << "-- average wait time: " << ceilTo3(waitTime / count) <<" ms "<<"("<<ceilTo3(cpuWaitTime/cpuWaitCount)<<" ms/"<<ceilTo3(ioWaitTime/ioWaitCount)<<" ms)"<< endl;
	outputFile << "-- average turnaround time: " << avgTurnaround(time, processes, n, num_cpu, cpuContextSwitch, ioContextSwitch, t_cs) << endl;
	outputFile << "-- number of context switches: " << contextSwitch << " (" << cpuContextSwitch << "/" << ioContextSwitch << ")" << std::endl;
	;
	outputFile << "-- number of preemptions: "<<preemption << " ("<<cpuPreemption<<"/"<<ioPreemption<<")"<< endl;
	outputFile << endl;
}

int main(int argc, char *argv[])
{

	if (argc < 8)
	{
		cerr << "ERROR: Not enough arguments" << endl;
		return -1;
	}

	// n = number of processes to simulate
	int num_processes = atoi(argv[1]);

	// n_cpu = number of processes that are CPU-bound
	int num_cpu = atoi(argv[2]);

	// seed = seed for pseudo RNG to determine interarrival times of burts
	long seed = atol(argv[3]);

	// lambda = exponential distribution parameter
	double lambda = atof(argv[4]);

	// upper_bound = upper bound for valid generated numbers
	int upper_bound = atoi(argv[5]);

	// context switch
	int t_cs = atoi(argv[6]);

	// constant alpha for SJF and SRT algorithms
	double alpha = atof(argv[7]);

	// time slice for RR algorithm
	int t_slice = atoi(argv[8]);

	srand48(seed);
	vector<Process> processes;

	// algorithm to create processes
	for (int i = 0; i < num_processes; ++i)
	{
		Process p;

		// initial parameters`~
		p.id = alphabets[i];
		p.turnaroundTime = 0;
		p.waitTime = 0;
		p.arrivalTime = floor(next_exp(lambda, upper_bound));
		p.numCpuBursts = ceil(drand48() * 64);
		p.tau = 1 / lambda;

		// determine whether a process is CPU-bound (any process created with index below num_cpu)
		if (i < num_processes - num_cpu)
			p.cpuBound = false;
		else
			p.cpuBound = true;

		// burst generation per process
		for (int j = 0; j < p.numCpuBursts; ++j)
		{
			if (j != p.numCpuBursts - 1)
			{
				int cpuBurstTime = ceil(next_exp(lambda, upper_bound));
				int ioBurstTime = ceil(next_exp(lambda, upper_bound)) * 10;
				if (i >= num_processes - num_cpu)
				{
					cpuBurstTime *= 4;
					ioBurstTime /= 8;
				}
				p.cpuBurstTime.push_back(cpuBurstTime);
				p.ioBurstTime.push_back(ioBurstTime);
			}
			else
			{
				int cpuBurstTime = ceil(next_exp(lambda, upper_bound));
				if (i >= num_processes - num_cpu)
				{
					cpuBurstTime *= 4;
				}
				p.cpuBurstTime.push_back(cpuBurstTime);
			}
		}
		processes.push_back(p);
	}

	// output part 1 information for each process
	cout << "<<< PROJECT PART I -- process set (n=" << num_processes << ") with " << num_cpu;
	if (num_cpu == 1)
	{
		cout << " CPU-bound process >>>" << endl;
	}
	else
	{
		cout << " CPU-bound processes >>>" << endl;
	}
	for (int i = 0; i < num_processes; i++)
	{
		if (i < num_processes - num_cpu)
			cout << "I/O-bound ";
		else
			cout << "CPU-bound ";
		cout << "process " << processes[i].id << ": arrival time " << processes[i].arrivalTime << "ms; " << processes[i].numCpuBursts << " CPU bursts"
			 << "\n";
	}
	std::cout << std::endl;

	// output part 2 information
	cout << "<<< PROJECT PART II -- t_cs=" << t_cs << "ms; alpha=" << alpha << "; t_slice=" << t_slice << "ms >>>" << endl; 

	string outputFileName = "simout.txt";

	// write to file
	ofstream outputFile(outputFileName);
	if (!outputFile.is_open())
	{
		cerr << "Error: Unable to open output file: " << outputFileName << std::endl;
		return 1;
	}

	// !--- MAIN FUNCTION CALLS ---!

	// FCFS call & output
	reset(processes, num_processes);
	FCFS(processes, num_processes, t_cs, num_cpu, outputFile);
	std::cout << std::endl;

	// // SJF call & output
	reset(processes, num_processes);
	SJF(processes, num_processes, t_cs, alpha, num_cpu, outputFile);
	std::cout << std::endl;

	for(int i = 0; i < num_processes; i++)
		processes[i].tau = 1 / lambda;

	// SRT call & output
	reset(processes, num_processes);
	SRT(processes, num_processes, t_cs, alpha, num_cpu, outputFile);
	std::cout << std::endl;

	reset(processes, num_processes);
	RR(processes, num_processes, t_cs, t_slice,  num_cpu, outputFile);

	outputFile.close();
}
