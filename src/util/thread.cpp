#include "thread.h"

Thread::Thread () {}
Thread::~Thread () {}

int Thread::run () { return 0; }
void *Thread::run_ (void *pthis_){
	Thread *pthis = (Thread *)pthis_;
	pthis->setRetval (pthis->run ());
	pthread_exit (NULL);
}
void Thread::start (){
	pthread_create (&myThread, NULL,
		Thread::run_, (void *)this);
}

int Thread::join (){
	pthread_join (myThread, NULL);
	return retval;
}
