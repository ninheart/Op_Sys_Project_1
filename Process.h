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
        this->processed = false;
    }
    char id;
    int arrivalTime;
    int nextArrivalTime;
    int timeLeft;
    int beginTime;
    int numCpuBursts;


    // temporary metrics
    int turnaroundTime;
    int waitTime;
    int cpuTime;
    int remainingTime;
    double tau;

    int step;
    bool inCPU;
    bool inQueue;
    bool inIO;
    bool swap;
    bool turn;
    bool cpuBound;
    bool preempt;
    bool processed;

    // metric tracking
    vector<int> cpuBurstTime;
    vector<int> ioBurstTime;
    vector<int> waitTimes;

    void reset(){
        this->waitTime = 0;
        this->cpuTime = 0;
        this->step = 0;
        this->inCPU =false;
        this->inQueue = false;
        this->inIO = false;
        this->swap = true;
        this->turn = false;
        this->processed = false;
        this->nextArrivalTime = this->arrivalTime;
        timeLeft = cpuBurstTime[step];
        waitTimes.clear();
    };

    bool operator==(Process const& other) const;
};

bool Process::operator==(Process const& other) const{
    return this->id == other.id;
}

#endif

