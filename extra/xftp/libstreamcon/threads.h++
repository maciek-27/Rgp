#ifndef _THREADS_H_
#define _THREADS_H_

#include "commons.h++"
#include <boost/thread/thread.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/thread/recursive_mutex.hpp>
#include <boost/thread/condition.hpp>

#include <pthread.h>

namespace streamcon {
	typedef boost::mutex Mutex;
	typedef boost::recursive_mutex RecursiveMutex;
	typedef boost::condition Condition;
	typedef boost::mutex::scoped_lock ScopedLock;
	typedef boost::recursive_mutex::scoped_lock RecursiveScopedLock;
	

	/*!
	  C++ style general thread class
	*/
	class Thread
	{
	private:
		pthread_t tid;
		void *arg;
		bool running;

		bool interrupt;

	public:
		__DE(CannotCreateThread, Exception);
		__DE(ThreadAlreadyRunning, Exception);
		
		Thread() throw();
		virtual ~Thread();
		void Start(void* arg) throw(CannotCreateThread,
									ThreadAlreadyRunning);
		void Start() throw(CannotCreateThread,
						   ThreadAlreadyRunning);
		void Kill() throw();
		void Wait() throw();
		int Id() throw();
	protected:
		void Run(void* arg);
		static void* EntryPoint(void* arg) throw();
		
		virtual void Setup();
		virtual void Clean();
		virtual void Execute(void* arg) = 0;

		bool Interrupt() {return interrupt;};
		void* Arg() const {return arg;}
		void Arg(void* a) {arg = a;}
	};
}

#endif // _THREADS_H_
