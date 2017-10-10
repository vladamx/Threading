#include "Event.h"
#include "Kernel.h"
#include <dos.h>

Event::Event(IVTNo ivt) {
	SysArgs args;
	args.call = 14;
	args.arg1 = FP_SEG(this);
	args.arg2 = FP_OFF(this);
	args.arg3 = ivt;
	Kernel::prepareCall(&args);
}

void Event::wait() {
	SysArgs args;
	args.call = 16;
	args.arg1 = FP_SEG(this);
	args.arg2 = FP_OFF(this);
	Kernel::prepareCall(&args);
}


void Event::signal() {
	SysArgs args;
	args.call = 17;
	args.arg1 = FP_SEG(this);
	args.arg2 = FP_OFF(this);
	Kernel::prepareCall(&args);
}

Event::~Event() {
	SysArgs args;
	args.call = 15;
	args.arg1 = FP_SEG(this);
	args.arg2 = FP_OFF(this);
	Kernel::prepareCall(&args);
}