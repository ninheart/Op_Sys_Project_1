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
    char id;
    int arrivalTime;
    int nextArrivalTime;
    int numCpuBursts;
    int turnaroundTime;
    int waitTime = 0;
    int cpuTime = 0;

    int step = 0;
    bool inCPU = false;
    bool inQueue = false;
    bool inIO = false;
    bool swap = true;
    bool turn = false;

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

