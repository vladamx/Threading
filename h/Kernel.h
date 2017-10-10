#ifndef H_KERNEL_H_
#define H_KERNEL_H_

#include "typedefs.h"

class PCB;
class KernelSem;

struct SysArgs {
	unsigned call;
	unsigned arg1;
	unsigned arg2;
	unsigned arg3;
	unsigned arg4;
};

class Kernel {
	
	private:
	static PCB* running;
	static PCB* mainT;
	static PCB* idleT;
	static pInterrupt oldSysCall;
	/*
	==========
	Interrupts
	==========
	*/
	static void replaceTimer();
	static void restoreTimer();
	static pInterrupt setSysCall();
	
	/*
	==========
	SysCalls
	==========
	*/
	/*
	--------
	Thread
	--------
	*/
	static void _newThread();
	static void _destroyThread();
	static void _startThread();
	static void _threadId();
	static void _runningId();
	static void _threadById();
	static void _dispatch();
	static void _waitToComplete();
	static void _exitThread();
	/*
	--------
	Semaphore
	--------
	*/
	static void _newSem();
	static void _destroySem();
	static void _semWait();
	static void _semSignal();
	static void _semVal();
	
	static int  semTimer(PCB*);
	static void semTimerHandler(KernelSem*);
	
	/*
	--------
	Events
	--------
	*/
	static void _newEvent();
	static void _eventWait();
	static void _eventSignal();
	static void _destroyEvent();
	

	public:
		
	static void runWrapper();
	static void prepareCall(SysArgs*);
	
	
	static void interrupt timer(...);
	static void interrupt sysCallRoutine(...);
	
	static void initialize();
	static void terminate();
	
	static PCB* getRunningPCB();
	static void registerNewPCB(PCB* pcb);
	static PCB* getPCBById(ID id); 
};


#endif /* H_KERNEL_H_ */
