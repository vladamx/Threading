#ifndef H_PCB_H_
#define H_PCB_H_

#include "typedefs.h"
#include "Queue.h"
class Thread;


class PCB{
private:
	friend class Kernel;
	friend class Thread;
	friend class Queue;
	friend class Semaphore;
	friend class KernelSem;
	friend class KerEv;
	
	enum Status {
		NEW,
		READY,
		BLOCKED,
		FINISHED
	};
	unsigned sp;
	unsigned ss;
	unsigned bp;
	unsigned* stack;
	static ID lastId;
		
	Time time;
	
	StackSize stackSize;
	Time timeSlice;
	Time semWaitTime;
	
	Thread* myThread;
	Queue<PCB>* waitingList;
	
	Status status;
	
	static void unblock(PCB*);
public:

	void unblockWaitingThreads();
	void newWaitingThread(PCB*);
	void createStack();

	ID id;
	
	PCB(StackSize stackSize, Time timSlice);
	~PCB();

};


#endif
