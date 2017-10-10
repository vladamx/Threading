#include "lib/Schedule.h"

#include "KerSem.h"
#include "Queue.h"
#include "PCB.h"

ID KernelSem::lastId = 1;

KernelSem::KernelSem(int init, Semaphore* mySem):counter(init), mySem(mySem), id(lastId++) {
	this->blockedPCBs = new Queue<PCB>();
}
KernelSem::~KernelSem() {
	delete this->blockedPCBs;
}

int KernelSem::wait(Time maxTimeToWait, PCB* running) {
	if(--this->counter < 0) {
		running->status = PCB::BLOCKED;
		if(maxTimeToWait > 0) {
			running->semWaitTime = maxTimeToWait;
		} else if(maxTimeToWait == 0) {
			running->semWaitTime = -1;
		}
		this->blockedPCBs->put(running);
		return 1;
	}
	return 0;
}

int KernelSem::signal() {
	if(this->counter++ < 0) {
		PCB* pcb = this->blockedPCBs->get();
		pcb->status = PCB::READY;
		Scheduler::put(pcb);
		return 1;
	}
	return 0;
}

int KernelSem::val () const {
	return this->counter;
}	