#include <iostream>
#include <vector>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <cmath>
#include <algorithm>
using namespace std;

char alphabets[] = {'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z'};

struct Process
{
    char id;
    int arrivalTime;
    int numCpuBursts;
    vector<int> cpuBurstTime;
    vector<int> ioBurstTime;
};

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

int main(int argc, char *argv[])
{

    if (argc < 6)
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

    srand48(seed);
    vector<Process> processes;

    // algorithm to create processes
    for (int i = 0; i < num_processes; ++i)
    {
        Process p;

        // initial parameters
        p.id = alphabets[i];
        p.arrivalTime = floor(next_exp(lambda, upper_bound));
        p.numCpuBursts = ceil(drand48() * 64);

        // burst generation per process
        for (int j = 0; j < p.numCpuBursts; ++j)
        {
            int cpuBurstTime = (ceil(next_exp(lambda, upper_bound)));
            int ioBurstTime = (ceil(next_exp(lambda, upper_bound))) * 10;

            if (i > num_cpu)
            {
                cpuBurstTime *= 4;
                ioBurstTime /= 8;
            }

            if (j != p.numCpuBursts - 1)
            {
                p.cpuBurstTime.push_back(cpuBurstTime);
                p.ioBurstTime.push_back(ioBurstTime);
            }
            else
            {
                p.cpuBurstTime.push_back(cpuBurstTime);
            }
        }
        processes.push_back(p);
    }


    // output information for each process
    std::cout << "<<< PROJECT PART I -- process set (n=" << num_processes << ") with " << num_cpu << " CPU-bound process >>>" << std::endl;
    for (int i = 0; i < num_processes; i++)
    {
        if(i <= num_cpu)
            std::cout << "I/O-bound ";
        else
            std::cout << "CPU-bound ";
        std::cout << "process " << processes[i].id << ": arrival time " << processes[i].arrivalTime << "ms; " << processes[i].numCpuBursts << " CPU bursts:" << "\n";
        for (int j = 0; j < processes[i].numCpuBursts; j++)
        {
            std::cout << "--> CPU burst " << processes[i].cpuBurstTime[j] << "ms";
            if (j != processes[i].numCpuBursts - 1)
            {
                std::cout << " --> I/O burst " << processes[i].ioBurstTime[j] << "ms";
            }
            std::cout << "\n";
        }
    }
}
