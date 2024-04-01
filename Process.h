#include <iostream>
#include <vector>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <cmath>
#include <algorithm>

using namespace std;

struct Process
{
public:
    char id;
    int arrivalTime;
    int numCpuBursts;
    int turnaroundTime = 0;
    int waitTime = 0;

    vector<int> cpuBurstTime;
    vector<int> ioBurstTime;

    void reset(){
        this->waitTime = 0;
        this->arrivalTime = 0;
    };
};

