#ifndef __event_h_
#define __event_h_

#include "typedefs.h"

// This is include by contract.
// User needs to include only Event.h in order to use Event system
#include "IVTEntry.h"

class KerEv;

class Event {
	friend class Kernel;
	
	public:
	Event(IVTNo);
	void wait();
	~Event();
	
	protected:
	friend class KerEv;
	friend class IVTEntry;
	void signal();
	
	private:
	KerEv* myImpl;
	
};

#endif