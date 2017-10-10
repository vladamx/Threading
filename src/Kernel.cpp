#include <iostream.h>
#include <dos.h>
#include "lib/SCHEDULE.h"
#include "lock.h"
#include "typedefs.h"

#include "Kernel.h"

#include "Queue.h"
#include "Queue.cpp"

#include "Event.h"
#include "IVTEntry.h"
#include "KerEv.h"
#include "KerSem.h"
#include "Semaphor.h"
#include "IdleT.h"

/*
======================================
All state Kernel needs for accounting
======================================
*/

pInterrupt Kernel::oldSysCall = Kernel::setSysCall();
volatile unsigned syncDispatch = 0;
unsigned dispatchPermitted = 1;
unsigned dispatchRequested = 0;

PCB* Kernel::running = 0;
PCB* Kernel::mainT = 0;
PCB* Kernel::idleT = 0;

unsigned * systemStack;
unsigned systemStackSize = 4096;
void (*calls[18])(void);

pInterrupt oldRoutine;

Queue<PCB>* pcbQueue = new Queue<PCB>();
Queue<KerEv>* eventQueue = new Queue<KerEv>();
Queue<KernelSem>* semQueue = new Queue<KernelSem>();

/*
=================
Lifecycle functions
=================
*/
void Kernel::initialize() {
	replaceTimer();

	setSysCall();
	
	running = new Thread(defaultStackSize, defaultTimeSlice)->myPCB;
	running->status = PCB::READY;
	idleT = new IdleThread(defaultStackSize, (Time)1)->myPCB;
	idleT->createStack();
	mainT = running;
	

}

void Kernel::terminate() {
	restoreTimer();
	delete pcbQueue;
	delete semQueue;
	delete eventQueue;
	delete running;
	running = 0;
	delete idleT;
	idleT = 0;
	mainT = 0;
}


pInterrupt Kernel::setSysCall(){
	pInterrupt oldSysCall = getvect(96);
	
	setvect(96,sysCallRoutine);

	calls[0] = _newThread;
	calls[1] = _destroyThread;
	calls[2] = _startThread;
	calls[3] = _waitToComplete;
	calls[4] = _threadId;
	calls[5] = _runningId;
	calls[6] = _threadById;
	calls[7] = _dispatch;
	calls[8] = _exitThread;
	calls[9] = _newSem;
	calls[10] = _destroySem;
	calls[11] = _semWait;
	calls[12] = _semSignal;
	calls[13] = _semVal;
	calls[14] = _newEvent;
	calls[15] = _destroyEvent;
	calls[16] = _eventWait;
	calls[17] = _eventSignal;

	systemStack = new unsigned[systemStackSize];
	
	return oldSysCall;
}

void Kernel::replaceTimer(){
	oldRoutine = getvect(8);
	setvect(8,timer);
}

void Kernel::restoreTimer(){
	setvect(8,oldRoutine);
}


/*
=================
Utilities
=================
*/

void Kernel::registerNewPCB(PCB* pcb) {
	pcbQueue->put(pcb);
}

void Kernel::prepareCall(SysArgs* args) {
	unsigned args_seg = FP_SEG(args);
	unsigned args_off = FP_OFF(args);
	asm {
		push cx
		push dx
		mov cx, args_seg
		mov dx, args_off
		int 60h
		pop dx
		pop cx
	}
}

PCB* Kernel::getPCBById(ID id) {
	return pcbQueue->findById(id);
}

void Kernel::runWrapper() {
	running->myThread->run();
	Thread::exit();
}

PCB* Kernel::getRunningPCB() {
	return running;
}

/*
=================
Timer
=================
*/

volatile unsigned tsp;
volatile unsigned tss;
volatile unsigned tbp;

extern void tick();


void interrupt Kernel::timer(...){
	// Pretty low level stuff. Ova rutina je zasticena od tajmera hardverskim zakljucavanjem.
	if(running->time < running->timeSlice) {
		running->time += 1;
	}
	
	// Make threads aware of time passing and deblock them if they waited too long
	if(semQueue->getSize() != 0) {
		semQueue->forEach(semTimerHandler);
	}
	
	oldRoutine();
	tick();
	
	if ((running->timeSlice == running->time) && (running->timeSlice > 0)) {
		
		if(!dispatchPermitted) {
			// Neke niti su privilegovane, vise cekaju, sistem
			dispatchRequested = 1;
		} else {
		
			asm {
				// Saving old context using temporary variables - Compiler constraint
				mov tsp, sp
				mov tss, ss
				mov tbp, bp
			}

			running->sp = tsp;
			running->ss = tss;
			running->bp = tbp;
			
			if(running->status == PCB::READY && running != idleT) {
				Scheduler::put(running);
			}
		
			// Delegating to Scheduler class to pick new running
			running = Scheduler::get();
			
			if(running == 0) {
				running = idleT;
			}

			tsp = running->sp;
			tss = running->ss;
			tbp = running->bp;
			
			running->time = 0;

			asm {
				// Promenio se procesorski kontekst. 
				// Sada se povlaci sa steka, ali sa steka niti, ne staticki prealociranog steka procesora
				mov sp, tsp
				mov ss, tss
				mov bp, tbp
			}
			
		}

	}
}

/*
=================
SysCall
=================
*/

volatile unsigned tcx, tdx;
volatile unsigned stsp, stss, stbp;
SysArgs* globalSysArgs = 0;

void interrupt Kernel::sysCallRoutine(...){
	// softverski zakljucamo da nas niko ne ometa dok radimo osetljive stvari
	dispatchPermitted = 0;
	
	//dovati argumente
	asm {
		mov tcx, cx
		mov tdx, dx
	}

	// Sacuvaj kontekst running niti
	asm {
		mov tsp, sp
		mov tss, ss
		mov tbp, bp
	}
	
	// Izvrsi prelazak na sistemski stek
	stss = FP_SEG(systemStack + systemStackSize);
	stsp = FP_OFF(systemStack + systemStackSize);
	asm {
		mov ss, stss
		mov sp, stsp
		mov bp, stsp
	}
	
	unlock;
	
	// Obradimo sistemski poziv
	globalSysArgs = (SysArgs*)MK_FP(tcx, tdx);
	calls[globalSysArgs->call]();
	
	lock;
	
	if(dispatchRequested == 1) {
		if(running) {
			running->sp = tsp;
			running->ss = tss;
			running->bp = tbp;

			if(running->status == PCB::READY && running != idleT) {
				Scheduler::put(running);
			}
		}
		running = Scheduler::get();
		if(running == 0) {
			running = idleT;
		}

		tsp = running->sp;
		tss = running->ss;
		tbp = running->bp;
				
		dispatchRequested = 0;
	}
	
	asm {
		mov sp, tsp
		mov ss, tss
		mov bp, tbp
	}
	
	
	dispatchPermitted = 1;

}


/*
=================
Synchronous dispatch
=================
*/

void Kernel::_dispatch() {
	dispatchRequested = 1;
}

/*
=================
Thread
=================
*/

void Kernel::_newThread() {
	Thread * newThread = (Thread*)MK_FP(globalSysArgs->arg1,globalSysArgs->arg2);
	newThread->myPCB = new PCB(globalSysArgs->arg3, globalSysArgs->arg4);
	newThread->myPCB->myThread = newThread;
	registerNewPCB(newThread->myPCB);
}

void Kernel::_destroyThread() {
	Thread * newThread = (Thread*)MK_FP(globalSysArgs->arg1,globalSysArgs->arg2);
	PCB * pcb = pcbQueue->get(newThread->myPCB);
	delete pcb;
}

void Kernel::_startThread() {
	Thread * thr = (Thread*)MK_FP(globalSysArgs->arg1,globalSysArgs->arg2);
	thr->myPCB->createStack();
	thr->myPCB->status = PCB::READY;
	Scheduler::put(thr->myPCB);
}

void Kernel::_waitToComplete() {
	Thread * newThread = (Thread*)MK_FP(globalSysArgs->arg1,globalSysArgs->arg2);
	if(newThread->myPCB->status != PCB::NEW && newThread->myPCB->status != PCB::FINISHED) {
		newThread->myPCB->newWaitingThread(running);
		dispatchRequested = 1;
	}
}

void Kernel::_exitThread() {	
	running->unblockWaitingThreads();
	
	running->status = PCB::FINISHED;
	
	dispatchRequested = 1;
}

void Kernel::_threadId() {
	Thread * newThread = (Thread*)MK_FP(globalSysArgs->arg1,globalSysArgs->arg2);
	globalSysArgs->arg3 = newThread->myPCB->id;
}

void Kernel::_runningId() {
	globalSysArgs->arg1 = running->id;
}


void Kernel::_threadById() {
	Thread * thr = getPCBById(globalSysArgs->arg1)->myThread;
	globalSysArgs->arg2 = FP_SEG(thr);
	globalSysArgs->arg3 = FP_OFF(thr);
}

/*
=================
Semaphore
=================
*/

int Kernel::semTimer(PCB* pcb) {
	if(pcb->semWaitTime > 0) {
		// Refresh threads on tick
		pcb->semWaitTime -= 1;
	}
	if(pcb->semWaitTime == 0) {
		pcb->status = PCB::READY;
		Scheduler::put(pcb);
		return 0;
	}
	return 1;
}

void Kernel::semTimerHandler(KernelSem * ks) {
	ks->blockedPCBs->filter(semTimer);
}

void Kernel::_newSem() {
	Semaphore * sem = (Semaphore*)MK_FP(globalSysArgs->arg1,globalSysArgs->arg2);
	sem->myImpl = new KernelSem(globalSysArgs->arg3, sem);
	semQueue->put(sem->myImpl);
}

void Kernel::_destroySem() {
	Semaphore * sem = (Semaphore*)MK_FP(globalSysArgs->arg1,globalSysArgs->arg2);
	KernelSem * ks = semQueue->get(sem->myImpl);
	delete ks;
}

void Kernel::_semWait() {
	Semaphore * sem = (Semaphore*)MK_FP(globalSysArgs->arg1,globalSysArgs->arg2);
	if(sem->myImpl->wait(/*maxTimeToWait*/globalSysArgs->arg3, running) == 1) {
		dispatchRequested = 1;
	}
}

void Kernel::_semSignal() {
	Semaphore * sem = (Semaphore*)MK_FP(globalSysArgs->arg1,globalSysArgs->arg2);
	sem->myImpl->signal();
}

void Kernel::_semVal() {
	Semaphore * sem = (Semaphore*)MK_FP(globalSysArgs->arg1,globalSysArgs->arg2);
	globalSysArgs->arg3 = sem->myImpl->val();
}

/*
=================
Event
=================
*/

void Kernel::_newEvent() {
	Event * ev = (Event*)MK_FP(globalSysArgs->arg1,globalSysArgs->arg2);
	ev->myImpl = IVTEntry::entries[globalSysArgs->arg3]->kv;
	ev->myImpl->owner = running;
	ev->myImpl->myEvent = ev;
	eventQueue->put(ev->myImpl);
}

void Kernel::_eventWait() {
	Event * ev = (Event*)MK_FP(globalSysArgs->arg1,globalSysArgs->arg2);
	if(ev->myImpl->wait(running) == 1) {
		dispatchRequested = 1;
	}
}

void Kernel::_eventSignal() {
	Event * ev = (Event*)MK_FP(globalSysArgs->arg1,globalSysArgs->arg2);
	if(ev->myImpl->signal() == 1) {
		dispatchRequested = 1;
	}
}

void Kernel::_destroyEvent() {
	Event * ev = (Event*)MK_FP(globalSysArgs->arg1,globalSysArgs->arg2);
	delete eventQueue->get(ev->myImpl);
}

