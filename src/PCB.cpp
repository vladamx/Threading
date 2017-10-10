#include <dos.h>
#include "lib/SCHEDULE.H"

#include "PCB.h"

#include "Kernel.h"

ID PCB::lastId = 1;	

PCB::PCB(StackSize stackSize, Time timeSlice):
		stackSize(stackSize),
		timeSlice(timeSlice),
		time(0),
		semWaitTime(0),
		id(PCB::lastId++),
		status(PCB::NEW),
		stack(0),
		sp(0),
		bp(0),
		ss(0) {
	if(this->id != 1) {
		this->waitingList = new Queue<PCB>();
	} else {
		this->waitingList = 0;
	}
}

void PCB::unblock(PCB * pcb) {
	pcb->status = PCB::READY;
	Scheduler::put(pcb);
}

void PCB::unblockWaitingThreads() {
	this->waitingList->forEach(unblock);
}

void PCB::newWaitingThread(PCB* running) {
	running->status = PCB::BLOCKED;
	this->waitingList->put(running);
}

void PCB::createStack(){
	unsigned* stack = new unsigned[this->stackSize];

	// Allow interrupts by setting I flag in PSW register. This allows us to be preemptive
	stack[this->stackSize - 1] =0x200;

	// Careful here. You need to put run wrapper so you can properly turn off the thread
	stack[this->stackSize - 2] = FP_SEG(Kernel::runWrapper);
	stack[this->stackSize - 3] = FP_OFF(Kernel::runWrapper);

	this->sp = FP_OFF(stack + this->stackSize - 12);

	// Entering interrupt routine
	this->ss = FP_SEG(stack + this->stackSize - 12);
	this->bp = this->sp;
	this->stack =stack;
}

PCB::~PCB() {
	delete [] stack;
	delete waitingList;
}
