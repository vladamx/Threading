#include "lock.h"
#include "Kernel.h"

extern int userMain(int, char*[]);


int main(int argc, char* argv [] ) {
	lock;
	Kernel::initialize();
	unlock;

	int returnArg = userMain(argc, argv);

	lock;
	Kernel::terminate();
	unlock;

	return returnArg;
}



