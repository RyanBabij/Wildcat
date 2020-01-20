#pragma once
#ifndef THREAD_THREAD_HPP
#define THREAD_THREAD_HPP

/*
#include <Thread/Thread.hpp>
Uses POSIX thread library to run multiple functions simultaneously. Currently only a basic wrapper which supports void functions with no args.
Designed for pthreadGC2.dll, pthread.h, sched.h, semaphore.h.
pthreadGC2.dll is supposed to be GCC build with c std non-error handling behaviour.

Compile with something like this: g++ Driver.cpp -o a.exe -I C:\Users\Ryan\Desktop\multicoreTest2  -I C:\Users\Ryan\Desktop\cod\lib\ libpthread-2.dll


*/

#include <pthread.h>
#include <Container/Vector/Vector.hpp>

/*
Functions passed to thread objects must be like this:
void* test(void*) {}
And are passed like this:
ThreadManager::add(&test,0);
*/


/*

    int pthread_create(pthread_t * thread, 
                       const pthread_attr_t * attr,
                       void * (*start_routine)(void *), 
                       void *arg);


    * thread - returns the thread id. (unsigned long int defined in bits/pthreadtypes.h)
    * attr - Set to NULL if default thread attributes are used. (else define members of the struct pthread_attr_t defined in bits/pthreadtypes.h) Attributes include:
          o detached state (joinable? Default: PTHREAD_CREATE_JOINABLE. Other option: PTHREAD_CREATE_DETACHED)
          o scheduling policy (real-time? PTHREAD_INHERIT_SCHED,PTHREAD_EXPLICIT_SCHED,SCHED_OTHER)
          o scheduling parameter
          o inheritsched attribute (Default: PTHREAD_EXPLICIT_SCHED Inherit from parent thread: PTHREAD_INHERIT_SCHED)
          o scope (Kernel threads: PTHREAD_SCOPE_SYSTEM User threads: PTHREAD_SCOPE_PROCESS Pick one or the other not both.)
          o guard size
          o stack address (See unistd.h and bits/posix_opt.h _POSIX_THREAD_ATTR_STACKADDR)
          o stack size (default minimum PTHREAD_STACK_SIZE set in pthread.h),
    * void * (*start_routine) - pointer to the function to be threaded. Function has a single argument: pointer to void.
    * *arg - pointer to argument of function. To pass multiple arguments, send a pointer to a structure.

*/




class Thread
{
	public:
	pthread_t thread;
	
	int start(void *(*function)(void*), void* arg)
	{ return pthread_create(&thread, 0, function, arg); }
	
	void waitUntilFinished()
	{ pthread_join(thread,0); }

};

/* Threadlock usage:

	class ThreadSafeClass
	{
		public:
		ThreadLock lock1;

		void doSomething()
		{
			lock1.lock();
			generateRandomNumber();
			lock1.unlock();
		}
	};

Function doSomething() is threadsafe, so it can be called by several threads at once. Non-threadsafe code can be given a threadsafe wrapper using this method.
*/

class ThreadLock
{
	public:
	pthread_mutex_t threadLock;
	
	ThreadLock()
	{ pthread_mutex_init(&threadLock,0); }
	
	/* Code enclosed between lock() and unlock() becomes thread safe, I think... */
	inline void lock()
	{ pthread_mutex_lock(&threadLock); }
	inline void unlock()
	{ pthread_mutex_unlock(&threadLock); }
};




/* Container for threads. */
class ThreadManager
{
	public:
	Vector <Thread*> vThread;
	
	/* Add a function to the container. The function takes no args. */
	/* Usage: ThreadGroup::add(&someFunction, 0); */
	void add(void *(*function)(void*), void* arg)
	{
		Thread* thread = new Thread;
		thread->start(function,arg);
		vThread.push(thread);
	}
	/* Makes sure all the threads are finished before we move on. */
	void finishAll()
	{
		for(int i=vThread.size()-1;i>=0;--i)
		{ vThread(i)->waitUntilFinished(); }
	}
};

#endif