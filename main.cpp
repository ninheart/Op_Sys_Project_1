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

// run the simulation; takes a vector of processes to simulate as well as the desired scheduler type
void runSimulation(std::vector<Process> processes, scheduler schedulerType, CPU &simCpu)
{
	// simulation time in ms
	int time = 0;

	// store processes in the simulation -- io is not necessarily a queue but it's easier naming convention
	std::vector<Process> readyQueue = std::vector<Process>();
	std::vector<Process> ioQueue = std::vector<Process>();

	// print message to signify beginning of simulation
	if (schedulerType == fcfs)
		std::cout << "time 0ms: Simulator started for FCFS ";
	if (schedulerType == sjf)
		std::cout << "time 0ms: Simulator started for SJF ";
	if (schedulerType == srt)
		std::cout << "time 0ms: Simulator started for SRT ";
	if (schedulerType == rr)
		std::cout << "time 0ms: Simulator started for RR ";
	printQueue(readyQueue);

	// main loop - must not be any processes operating
	while (!readyQueue.empty() || !ioQueue.empty() || !processes.empty())
	{
		// check for process arrival
		for (int i = 0; i < processes.size(); i++)
		{
			if (processes[i].arrivalTime == time)
			{
				// add to ready queue and remove from the input vector
				readyQueue.push_back(processes[i]);
				processes.erase(processes.begin() + i);
				i--;

				// print arrival message
				std::cout << "time " << time << "ms: Process " << readyQueue.back().id << " arrived ";
				printQueue(readyQueue);
			}
		}

		// check for io completion (and put processes back in ready queue)

		// check whatever else is needed (haven't looked yet)

		// increment process time
		time++;
	}

	// print message to signify ending of simulation
	if (schedulerType == fcfs)
		std::cout << "time " << time << "ms: Simulator ended for FCFS ";
	if (schedulerType == sjf)
		std::cout << "time " << time << "ms: Simulator ended for SJF ";
	if (schedulerType == srt)
		std::cout << "time " << time << "ms: Simulator ended for SRT ";
	if (schedulerType == rr)
		std::cout << "time " << time << "ms: Simulator ended for RR ";

	// output statistics
}

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

string ceilTo3(double value){
	double roundedValue = std::ceil(value * 1000.0) / 1000.0;
	stringstream ss;
	ss << std::fixed << std::setprecision(3) << roundedValue;
	return ss.str();
}

string cpuUtilization(int time, vector<Process> processes, int n){
	double totalBurstTime = 0;
	for(int i = 0; i < n; ++i){
		for(int j = 0; j < processes[i].cpuBurstTime.size(); ++j){
			totalBurstTime += processes[i].cpuBurstTime[j];
		}
	}
	return ceilTo3(totalBurstTime / time * 100);
}

void FCFS(vector<Process> &processes, int n, int t_cs, const string &outputFileName)
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

					// swith out process
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

	// write to file
	ofstream outputFile(outputFileName);
	if (!outputFile.is_open())
	{
		cerr << "Error: Unable to open output file: " << outputFileName << std::endl;
		return;
	}

	outputFile <<"Algorithm FCFS"<<endl;
	outputFile <<"-- CPU utilization: "<<cpuUtilization(time,processes,n)<<"%"<<endl;
	outputFile <<"-- average CPU burst time: "<<endl;

	outputFile.close();

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

	// create CPU and scheduler for simulation
	CPU simCpu = CPU();
	scheduler simScheduler;

	string outputFileName = "simout.txt";

	reset(processes, num_processes);
	FCFS(processes, num_processes, t_cs, outputFileName);

	// simulate every scheduling algorithm
	// simScheduler = fcfs;
	// runSimulation(processes, simScheduler, simCpu);

	// simScheduler = sjf;
	// runSimulation(processes, simScheduler, simCpu);

	// simScheduler = srt;
	// runSimulation(processes, simScheduler, simCpu);
}
