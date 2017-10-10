#include <dos.h>

#include "KerEv.h"
#include "Event.h"
#include "IVTEntry.h"

IVTEntry* IVTEntry::entries[256] = {0};

IVTEntry::IVTEntry(IVTNo numEntry, pInterrupt newRoutine){
	if((int)numEntry >= 0 && (int)numEntry <= 255) {
		this->routine = getvect(numEntry);
		setvect(numEntry,newRoutine);
		this->numEntry = numEntry;
		entries[numEntry] = this;
		this->kv = new KerEv();
	}
}

void IVTEntry::signal() {
	this->kv->myEvent->signal();
}

void IVTEntry::callOld() {
	this->routine();
}


