#ifndef __ivtentry_h_
#define __ivtentry_h_

#include "typedefs.h"

class KerEv;

class IVTEntry {
	friend class Kernel;

	public:
	IVTEntry(IVTNo, pInterrupt);
	void signal();
	void callOld();
	
	private:
	static IVTEntry* entries[256];
	
	KerEv* kv;
	pInterrupt routine;
	IVTNo numEntry;
};

#define PREPAREENTRY(numEntry, old)\
void interrupt inter##numEntry(...); \
IVTEntry newEntry##numEntry(numEntry, inter##numEntry); \
void interrupt inter##numEntry(...) {\
	newEntry##numEntry.signal();\
	if (old == 1)\
	newEntry##numEntry.callOld();\
}


#endif