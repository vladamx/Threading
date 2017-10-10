#include <dos.h>
#include "Semaphor.h"
#include "PCB.h"
#include "Kernel.h"


Semaphore::Semaphore(int init){
	SysArgs args;
	args.call = 9;
	args.arg1 = FP_SEG(this);
	args.arg2 = FP_OFF(this);
	args.arg3 = init;
	Kernel::prepareCall(&args);
}


Semaphore::~Semaphore() {
	SysArgs args;
	args.call = 10;
	args.arg1 = FP_SEG(this);
	args.arg2 = FP_OFF(this);
	Kernel::prepareCall(&args);
}

int Semaphore::wait(Time maxTimeToWait) {
	SysArgs args;
	args.call = 11;
	args.arg1 = FP_SEG(this);
	args.arg2 = FP_OFF(this);
	args.arg3 = maxTimeToWait;
	Kernel::prepareCall(&args);
	if(Kernel::getRunningPCB()->semWaitTime == 0) {
		return 0;
	} 
	return 1;
}

void Semaphore::signal() {
	SysArgs args;
	args.call = 12;
	args.arg1 = FP_SEG(this);
	args.arg2 = FP_OFF(this);
	Kernel::prepareCall(&args);
}

int Semaphore::val() const {
	SysArgs args;
	args.call = 13;
	args.arg1 = FP_SEG(this);
	args.arg2 = FP_OFF(this);
	Kernel::prepareCall(&args);
	return args.arg3;
}