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
	Process *currentProcess = NULL; //current process running in cpu
	Process *switchingProcess = NULL;

	int context = 0;  //context switch

	void addProcess(const Process &process)
	{
		processQueue.push_back(process);
	}

	// prints the queue in the specified format

    Process front(){
		return processQueue.front();
	}

	void popFront(){
		return processQueue.pop_front();
	}

	int getQueueSize(){
		return processQueue.size();
	}


	void printQueue()
	{
		cout << "[Q";
		if (processQueue.empty())
		{
			cout << " <empty>]" << endl;
		}
		else
		{
			for (auto i = processQueue.begin(); i != processQueue.end(); ++i)
			{
				cout << ' '<<(*i).id;
			}
			cout << "]" << endl;
		}
	}
};

#endif