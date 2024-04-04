#include <deque>

#include "Process.h"

#ifndef CPU_H
#define CPU_H


using namespace std;

class CPU
{
private:

	// ready queue
	deque<Process> processQueue;

public:

	// construct a CPU
	CPU()
	{
		currentProcess = NULL;
		switchingProcess = NULL;
		context = 0;
	}

	// running process
	Process *currentProcess;

	// process to be switched into the CPU
	Process *switchingProcess;

	// context switch
	int context;

	// add to ready queue
	void addProcess(const Process &process)
	{
		processQueue.push_back(process);
	}

	// return process at front of ready queue
    Process front(){
		return processQueue.front();
	}

	// remove process from front of ready queue
	void popFront(){
		return processQueue.pop_front();
	}

	// return size of ready queue
	int getQueueSize(){
		return processQueue.size();
	}

	// prints the queue in the specified format
	void printQueue()
	{
		cout << "[Q";
		if (processQueue.empty())
		{
			cout << " <empty>]" << endl;
		}
		else
		{
			for (int i = 0; i < processQueue.size(); ++i)
			{
				cout << ' '<< (processQueue[i]).id;
			}
			cout << "]" << endl;
		}
	}

	// return reference to process queue
	deque<Process>* getProcessQueue() { return &processQueue;}
};

#endif