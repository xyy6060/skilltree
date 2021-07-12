#ifndef __COMMON_THREAD_H__
#define __COMMON_THREAD_H__

#include <pthread.h>
#include <sys/time.h>
#include <time.h>
#include <errno.h>

namespace modCommon
{
	class Mutex
	{
		pthread_mutex_t _mut;

	public:
		Mutex() {pthread_mutex_init(&_mut, NULL);}
		~Mutex() {pthread_mutex_destroy(&_mut);}

		void lock() {pthread_mutex_lock(&_mut);}
		void unlock() {pthread_mutex_unlock(&_mut);};
	};

	class AutoLock
	{
		Mutex& _mut;
	public:
		AutoLock(Mutex& mut):_mut(mut) {_mut.lock();}
		~AutoLock() {_mut.unlock();}
	};

	template <typename T>
	class Condition
	{
		T _t;
		pthread_mutex_t _mut;
		pthread_cond_t _cond;
		
	public:
		Condition(const T& t):_t(t)
		{
			pthread_mutex_init(&_mut,NULL);
	    	pthread_cond_init(&_cond,NULL);
		}

		~Condition()
		{
		    pthread_cond_destroy(&_cond);
	    	pthread_mutex_destroy(&_mut);
		}

		void set(const T& newval)
		{
			pthread_mutex_lock(&_mut);
			_t = newval;
			if(_t)
		  		pthread_cond_broadcast(&_cond);
			pthread_mutex_unlock(&_mut);
		}

		T get()
		{
			T val;
			
			pthread_mutex_lock(&_mut);
			val = _t;
			pthread_mutex_unlock(&_mut);
			
			return val;
		}

		void cond_wait()
		{
			pthread_mutex_lock(&_mut);
			while(!_t)
			{
				pthread_cond_wait(&_cond,&_mut);
			}
			
			pthread_mutex_unlock(&_mut);
		}

		bool cond_timedwait(unsigned long msec)
		{
			struct timeval now;
			struct timespec timeout;
			int retcode = 0;
			bool ret = false;

			gettimeofday(&now, NULL);
			timeout.tv_sec = now.tv_sec + (msec / 1000);
			timeout.tv_nsec = (now.tv_usec + (msec % 1000)*1000)*1000;
			if(timeout.tv_nsec >= 1000000000)
			{
				timeout.tv_sec++;
				timeout.tv_nsec -= 1000000000;
			}

			pthread_mutex_lock(&_mut);
			while(!_t && !retcode)
			{
				retcode = pthread_cond_timedwait(&_cond,&_mut, &timeout);
			}

			if(_t) ret = true;
			pthread_mutex_unlock(&_mut);

			return ret;
		}
	};

}
#endif

