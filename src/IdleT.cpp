#include "IdleT.h"

IdleThread::IdleThread(StackSize stack_size, Time time_slice):Thread(stack_size,time_slice) {
}

void IdleThread::run() {
	while(1);
}
