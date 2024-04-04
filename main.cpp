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
	for (int i = 0; i < queue.size(); i++)
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

// compare two processes by least tau value (and if they are equal, by least ID)
bool compareTau(const Process &p1, const Process &p2)
{
	// Compare tau if not equal, otherwise the id
	if(p1.tau != p2.tau)
		return p1.tau < p2.tau;
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
		for(int j = 0; j < processes[i].cpuBurstTime.size(); ++j){
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
		for(int j = 0; j < processes[i].cpuBurstTime.size(); j++)
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
	return ceilTo3(avgBurst / burstCount) + " ms (" + ceilTo3(avgIOBoundBurst / ioBurst) + " ms/" + ceilTo3(avgCPUBoundBurst / cpuBurst) + " ms)";
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
		for(int j = 0; j < processes[i].waitTimes.size(); j++)
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
	return ceilTo3(avgBurst / burst) + " ms (" + ceilTo3(avgIOBoundBurst / ioBurst) + " ms/" + ceilTo3(avgCPUBoundBurst / cpuBurst) + " ms)";
}

// calculates the average process turnaround time for a group of processes and reutrns it in the specified format as a String
string avgTurnaround(int time, vector<Process> processes, int n, int num_cpu, int cpuCS, int ioCS, double t_cs)
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
	for(int i = 0; i < n; i++)
	{
		for(int j = 0; j < processes[i].waitTimes.size(); j++)
		{
			if(processes[i].cpuBound)
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

	// Add in # of switches
	avgCPUBoundBurst += cpuCS * t_cs;
	avgIOBoundBurst += ioCS * t_cs;
	avgBurst += (ioCS + cpuCS) * t_cs;

	return ceilTo3(avgBurst / burst) + " ms (" + ceilTo3(avgIOBoundBurst / ioBurst) + " ms/" + ceilTo3(avgCPUBoundBurst / cpuBurst) + " ms)";
}

void FCFS(vector<Process> &processes, int n, int t_cs, ofstream &outputFile)
{
	sort(processes.begin(), processes.end(), compareArrivalTime);

	int time = 0;
	int alive = n;
	double waitTime = 0;
	double turnaroundTime = 0;
	int contextSwtich = 0;

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

				cout<<"time "<<time<<"ms: ";
				cout << "Process " << temp->id << " started using the CPU for "<<temp->cpuBurstTime[temp->step]<<"ms burst ";
				cpu.printQueue();
				contextSwtich +=1;
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


				if (p->inIO)
				{
					p->inIO = false;
					p->step++;
					cout << "time " << time << "ms: ";
					cout<<"Process "<< p->id <<" completed I/O; added to ready queue ";
					// cout<<cpu.currentProcess->id<<endl;
					cpu.printQueue();
				}else{
					cout << "time " << time << "ms: ";
					cout << "Process " << p->id << " arrived; added to ready queue ";
					cpu.printQueue();
				}
			}

			if (p->inQueue)
			{
				// if there's no other process runnning then run this process
				if (cpu.currentProcess == NULL && cpu.switchingProcess == NULL && *p == cpu.front())
				{
					p->inQueue = false;
					p->swap = true;
					waitTime += p->waitTime;
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
					// p->step++;
					p->nextArrivalTime = updateArrivalTime;

					cout<<"time "<<time<<"ms: Process "<<p->id<<" completed a CPU burst; "<<p->cpuBurstTime.size()-p->step-1;
					if (p->cpuBurstTime.size() - p->step - 1 == 1){
						cout << " burst to go ";
					}else{
						cout << " bursts to go ";
					}
						
					cpu.printQueue();

					cout << "time " << time << "ms: Process " << p->id << " switching out of CPU; blocking on I/O until time " << updateArrivalTime<<"ms ";
					cpu.printQueue();

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

	outputFile <<"Algorithm FCFS"<< std::endl;
	outputFile <<"-- CPU utilization: "<<cpuUtilization(time,processes,n)<<"%"<< std::endl;
	outputFile <<"-- average CPU burst time: "<< std::endl;
	outputFile << std::endl;

}

// SJF algorithm
void SJF(vector<Process> &processes, int n, int t_cs, double alpha, int num_cpu, ofstream &outputFile)
{
	sort(processes.begin(), processes.end(), compareArrivalTime);

	int time = 0;
	int alive = n;
	double waitTime = 0;
	double turnaroundTime = 0;
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
				cpu.popFront();
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
				cpu.addProcess(*p);
				p->beginTime = time;
				p->inQueue = true;
				deque<Process>* q = cpu.getProcessQueue();
				deque<Process> q2 = deque<Process>();
				for(int i = 0; i < q->size(); i++)
					q2.push_back(q->at(i));
				if(p->inIO)
				{
					p->inIO = false;
					p->step++;
					if(time < 10000)
						cout << "time " << time << "ms: " <<"Process "<< p->id <<" (tau " << p->tau << "ms) completed I/O; added to ready queue ";

					// sort queue by tau
					std::sort(q2.begin(), q2.end(), compareTau);
					if(time < 10000)
						cpu.printQueue();
				}
				else
				{
					if(time < 10000)
						cout << "time " << time << "ms: " << "Process " << p->id << " (tau " << p->tau << "ms) arrived; added to ready queue ";

					// sort queue by tau
					std::sort(q2.begin(), q2.end(), compareTau);
					if(time < 10000)
						cpu.printQueue();
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
					waitTime += p->waitTime;
					cpu.context += t_cs / 2;
					cpu.switchingProcess = p;
					p->cpuTime = 0;
					p->waitTimes.push_back(p->waitTime);
					p->waitTime = 0;
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
	outputFile << "-- number of context switches: " << contextSwitch << " (" << ioContextSwitch << "/" << cpuContextSwitch << ")" << std::endl;
	outputFile << "-- number of preemptions: 0 (0/0)" << std::endl;
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

		// initial parameters
		p.id = alphabets[i];
		p.turnaroundTime = 0;
		p.waitTime = 0;
		p.arrivalTime = floor(next_exp(lambda, upper_bound));
		p.numCpuBursts = ceil(drand48() * 64);
		p.tau = 1 / lambda;

		// determine whether a process is CPU-bound (any process created with index below num_cpu)
		if(i < num_cpu + 1)
			p.cpuBound = true;
		else
			p.cpuBound = false;

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
	FCFS(processes, num_processes, t_cs, outputFile);
	std::cout << std::endl;

	// SJF call & output
	reset(processes, num_processes);
	SJF(processes, num_processes, t_cs, alpha, num_cpu, outputFile);
	std::cout << std::endl;

	outputFile.close();

}
