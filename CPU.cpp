#include "CPU.h"

CPU::CPU()
{
	idle = true;
}

bool CPU::getIdle() { return this->idle; }

void CPU::setProcess(Process addedProcess) { this->process = addedProcess; }