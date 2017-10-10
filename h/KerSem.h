#ifndef _kernelsem_h_
#define _kernelsem_h_

#include "typedefs.h"
#include <PCB.h>

class Queue;
class Semaphore;

class KernelSem{
	friend class Kernel;
	friend class PCB;
	
	private:
	
	int counter;
	Queue<PCB>* blockedPCBs;
	Semaphore* mySem;
	static ID lastId;
	
	public:
	// For now! Refactor later. Queue access needs to change
	ID id;
	
	KernelSem(int, Semaphore*);
	~KernelSem();
	
	int wait(Time maxTimeToWait, PCB* running);
	int signal();
	int val () const; 
};

#endif