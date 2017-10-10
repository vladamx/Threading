#ifndef H_IDLE_H_
#define H_IDLE_H_

#include "typedefs.h"

#include "Thread.h"

class IdleThread: public Thread {
public:
	IdleThread(StackSize,Time);
	void run();
};


#endif /* H_LOCK_H_ */