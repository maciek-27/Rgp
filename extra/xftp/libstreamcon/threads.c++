#include "threads.h++"
#include "throw.h++"
#include <signal.h>

using namespace streamcon;

Thread::Thread() throw()
{
	running = false;
	interrupt = false;
}

Thread::~Thread()
{
	Kill();
	pthread_detach(tid); // free some hidden thread stack memory
}

void Thread::Start(void *arg) throw(CannotCreateThread,
									ThreadAlreadyRunning)
{
	interrupt = false;
	if(running)
		THROW(ThreadAlreadyRunning);
	Arg(arg); // store user data
	if(pthread_create(&tid, NULL, Thread::EntryPoint, this) < 0) {
		THROW(CannotCreateThread);
	}
}

void Thread::Start() throw(CannotCreateThread,
						   ThreadAlreadyRunning)
{
	Start(NULL);
}

void Thread::Kill() throw()
{
	if(running) {
		interrupt = true;
		pthread_join(tid, NULL);
	}
}

void Thread::Wait() throw()
{
	if(running)
		pthread_join(tid, NULL);
}

int Thread::Id() throw()
{
	return tid;
}

void Thread::Run(void * arg)
{
	Setup();
	running = true;
	Execute(arg);
	running = false;
	Clean();
}

void* Thread::EntryPoint(void* pthis) throw()
{
	Thread* pt = (Thread*)pthis;
	pt->Run(pt->Arg());
	return NULL;
}

void Thread::Setup()
{
	
}

void Thread::Clean()
{

}
