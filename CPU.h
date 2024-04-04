#include <deque>

#include "Process.h"

#ifndef CPU_H
#define CPU_H


using namespace std;

class CPU
{
private:
	deque<Process> processQueue;

public:
	CPU()
	{
		currentProcess = NULL;
		switchingProcess = NULL;
		context = 0;
	}
	Process *currentProcess; //current process running in cpu
	Process *switchingProcess;

	int context;  //context switch

	void addProcess(const Process &process)
	{
		processQueue.push_back(process);
	}

    Process front(){
		return processQueue.front();
	}

	void popFront(){
		return processQueue.pop_front();
	}

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

	deque<Process>* getProcessQueue() { return &processQueue;}
};

#endif