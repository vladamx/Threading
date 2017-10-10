#include "KerEv.h"
#include "lib/SCHEDULE.h"

ID KerEv::lastId = 1;

KerEv::KerEv():value(0),id(lastId++) {	
}

int KerEv::wait(PCB* running) {
	if((this->owner == running)) {
		if(this->value == 0) {
		  	running->status = PCB::BLOCKED;
			this->blockedThread = running;
			return 1;
		} else {
			this->value = 0;
		}
	}
	return 0;
}

int KerEv::signal() {
	if(this->blockedThread == 0) {
		this->value = 1;
		return 0;
	} else {
		this->blockedThread->status = PCB::READY;
		Scheduler::put(this->blockedThread);
		this->blockedThread = 0;
		return 1;
	}
}