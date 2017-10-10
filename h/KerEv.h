#ifndef __kerevent_h_
#define __kerevent_h_

#include "typedefs.h"

#include "PCB.h"

class Event;

class KerEv {
	friend class Kernel;
	friend class IVTEntry;
	
	public:
	
	KerEv();
	
	int wait(PCB*);
	int signal();

	// Queue expects ids for data strucures.
	// Let this puvlic for now, refactor later
	ID id;
	
	private:
	static ID lastId;

	int value;
	Event* myEvent;
	PCB* blockedThread;
	PCB* owner;
	
};


#endif