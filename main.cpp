#include <iostream>
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
enum scheduler { fcfs, sjf, srt, rr };

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

// prints the queue in the specified format
void printQueue(std::vector<Process> &queue)
{
	std::cout << "[Q";
	if(queue.size() == 0)
	{
		std::cout << " <empty>]" << std::endl;
		return;
	}
	for(int i = 0; i < queue.size(); i++)
		std::cout << " " << queue[i].id;
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
	if(schedulerType==fcfs) std::cout << "time 0ms: Simulator started for FCFS ";
	if(schedulerType==sjf) std::cout << "time 0ms: Simulator started for SJF ";
	if(schedulerType==srt) std::cout << "time 0ms: Simulator started for SRT ";
	if(schedulerType==rr) std::cout << "time 0ms: Simulator started for RR ";
	printQueue(readyQueue);

	// main loop - must not be any processes operating
	while(!readyQueue.empty() || !ioQueue.empty() || !processes.empty())
	{
		// check for process arrival
		for(int i = 0; i < processes.size(); i++)
		{
			if(processes[i].arrivalTime == time)
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
	if(schedulerType==fcfs) std::cout << "time " << time << "ms: Simulator ended for FCFS ";
	if(schedulerType==sjf) std::cout << "time " << time << "ms: Simulator ended for SJF ";
	if(schedulerType==srt) std::cout << "time " << time << "ms: Simulator ended for SRT ";
	if(schedulerType==rr) std::cout << "time " << time << "ms: Simulator ended for RR ";

	// output statistics
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
	for(int i = 0; i < num_processes; ++i)
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
	cout << "<<< PROJECT PART II -- t_cs=" << t_cs << "ms; alpha=" << alpha<<"; t_slice="<<t_slice<<"ms >>>"<<endl;

	// create CPU and scheduler for simulation
	CPU simCpu = CPU();
	scheduler simScheduler;

	// simulate every scheduling algorithm
	simScheduler = fcfs;
	runSimulation(processes, simScheduler, simCpu);

	simScheduler = sjf;
	runSimulation(processes, simScheduler, simCpu);

	simScheduler = srt;
	runSimulation(processes, simScheduler, simCpu);

}
