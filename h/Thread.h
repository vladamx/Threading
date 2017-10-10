#ifndef _thread_h_
#define _thread_h_

#include "typedefs.h"

const StackSize defaultStackSize = 4096;
const Time defaultTimeSlice = 2; // default = 2*55ms

class PCB; // Kernel's implementation of a user's thread

class Thread {
public:
  void start();
  void waitToComplete();
  virtual ~Thread();
  ID getId();
  static ID getRunningId();

  
  static Thread * getThreadById(ID id);

protected:
  friend class PCB;
  friend class Kernel;
  Thread (StackSize stackSize = defaultStackSize, Time timeSlice = defaultTimeSlice);
  virtual void run() {}


private:
  static void exit();
  PCB* myPCB;
};

void dispatch ();

#endif
