#include <iostream.h>
#include <dos.h>

#include "Kernel.h"
#include "Thread.h"

Thread::Thread(StackSize stack_size, Time time_slice){
	SysArgs args;
	args.call = 0;
	args.arg1 = FP_SEG(this);
	args.arg2 = FP_OFF(this);
	args.arg3 = stack_size;
	args.arg4 = time_slice;
	Kernel::prepareCall(&args);
}


Thread::~Thread() {
	SysArgs args;
	args.call = 1;
	args.arg1 = FP_SEG(this);
	args.arg2 = FP_OFF(this);
	Kernel::prepareCall(&args);
}

void Thread::start() {
	SysArgs args;
	args.call = 2;
	args.arg1 = FP_SEG(this);
	args.arg2 = FP_OFF(this);
	Kernel::prepareCall(&args);
}

void Thread::waitToComplete() {
	SysArgs args;
	args.call = 3;
	args.arg1 = FP_SEG(this);
	args.arg2 = FP_OFF(this);
	Kernel::prepareCall(&args);
}

ID Thread::getId() {
	SysArgs args;
	args.call = 4;
	args.arg1 = FP_SEG(this);
	args.arg2 = FP_OFF(this);
	Kernel::prepareCall(&args);
	return args.arg3;
}

ID Thread::getRunningId() {
	SysArgs args;
	args.call = 5;
	Kernel::prepareCall(&args);
	return args.arg1;
}

Thread* Thread::getThreadById(ID id) {
	SysArgs args;
	args.call = 6;
	args.arg1 = id;
	Kernel::prepareCall(&args);
	return (Thread*)MK_FP(args.arg2,args.arg3);
}

void dispatch() {
	SysArgs args;
	args.call = 7;
	Kernel::prepareCall(&args);
}

void Thread::exit(){
	SysArgs args;
	args.call = 8;
	Kernel::prepareCall(&args);
}