#include <iostream>
#include <vector>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <cmath>
#include <algorithm>

using namespace std;

#ifndef PROCESS
#define PROCESS

struct Process
{
public:

    Process()
    {
        this->waitTime = 0;
        this->cpuTime = 0;
        this->step = 0;
        this->inCPU =false;
        this->inQueue = false;
        this->inIO = false;
        this->swap = true;
        this->turn = false;
    }
    char id;
    int arrivalTime;
    int nextArrivalTime;
    int numCpuBursts;
    int turnaroundTime;
    int waitTime;
    int cpuTime;

    int step;
    bool inCPU;
    bool inQueue;
    bool inIO;
    bool swap;
    bool turn;

    vector<int> cpuBurstTime;
    vector<int> ioBurstTime;

    void reset(){
        this->waitTime = 0;
        this->nextArrivalTime = this->arrivalTime;
    };

    bool operator==(Process const& other) const;
};

bool Process::operator==(Process const& other) const{
    return this->id == other.id;
}

#endif

