#include "Process.h"

#ifndef CPU_H
#define CPU_H

class CPU
{
public:

	// constructor
	CPU();

	// getter
	bool getIdle();

	// setter
	void setProcess(Process addedProcess);

private:

	bool idle;
	Process process;
};


#endif