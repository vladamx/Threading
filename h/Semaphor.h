#ifndef _semaphor_h_
#define _semaphor_h_

#include "typedefs.h"

class KernelSem;

class Semaphore {
	friend class Kernel;
	friend class PCB;
	public:
	Semaphore(int init=1);
	virtual ~Semaphore();
	
	virtual int wait(Time maxTimeToWait);
	virtual void signal();
	
	int val () const; // Returns current value of semaphore
	
	
	private:
	KernelSem* myImpl;
};

#endif