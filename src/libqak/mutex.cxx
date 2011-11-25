// vim: set ts=4 sw=4 tw=120:
//=====================================================================================================================|
//
//	Copyright (c) 2011, Marsh Ray
//
//	Permission to use, copy, modify, and/or distribute this software for any
//	purpose with or without fee is hereby granted, provided that the above
//	copyright notice and this permission notice appear in all copies.
//
//	THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
//	WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
//	MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
//	ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
//	WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
//	ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT
//	OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
//
//=====================================================================================================================|
//
//	libqak/mutex.cxx

#include "qak/mutex.hxx"

#define IMPLEMENTATION_PTHREAD 1

#if IMPLEMENTATION_PTHREAD

//		Error code definitions from The Open Group - Single UNIX® Specification, Version 2
#	include <errno.h> // pthread error codes
#	include <pthread.h>

#elif 0
#elif 1
#	error ""
#endif

#include <cassert>

namespace qak { //=====================================================================================================|

	mutex_lock::mutex_lock(
		mutex & m
	) :
		p_m_(0)
	{
		mutex_lock ml = m.lock();
		std::swap(ml.p_m_, p_m_);
	}

	//-----------------------------------------------------------------------------------------------------------------|

	mutex_lock::mutex_lock(
		private_construct_tag const &,
		mutex * p_m
	) :
		p_m_(p_m)
	{ }

	//-----------------------------------------------------------------------------------------------------------------|

	mutex_lock::~mutex_lock()
	{
		if (p_m_)
		{
#if IMPLEMENTATION_PTHREAD

			int err = ::pthread_mutex_unlock(
				reinterpret_cast<pthread_mutex_t *>(p_m_->imp_) ); // pthread_mutex_t *mutex
			if (err)
			{
				//char const * psz = 0;
				//switch (err)
				//{				Error code definitions from The Open Group - Single UNIX® Specification, Version 2
				//case EINVAL:
				//	psz = "The value specified by mutex does not refer to an initialised mutex object.";
				//	break;
				//case EAGAIN:
				//	psz = "The mutex could not be acquired because the maximum number of recursive locks for mutex "
				//		"has been exceeded.";
				//	break;
				//case EPERM:
				//	psz = "The current thread does not own the mutex.";
				//	break;
				//}
				throw 0;
			}

#elif 0
#elif 1
#	error ""
#endif
		}
	}

	//-----------------------------------------------------------------------------------------------------------------|

	mutex_lock::mutex_lock(mutex_lock && that) : p_m_(that.p_m_)
	{
		assert(p_m_);
		that.p_m_ = 0;
	}

	//-----------------------------------------------------------------------------------------------------------------|

	bool mutex_lock::is_locking(mutex const & m) const
	{
		return &m == p_m_;
	}

	//-----------------------------------------------------------------------------------------------------------------|

	mutex::mutex() // : imp_ not initialized
	{
#if IMPLEMENTATION_PTHREAD

		static_assert(sizeof(pthread_mutex_t) <= sizeof(imp_), "increase size of imp_" );
		static_assert(alignof(pthread_mutex_t) <= alignof(imp_), "increase align of imp_" );

		//	http://pubs.opengroup.org/onlinepubs/007908799/xsh/pthread_mutex_init.html
		int err = ::pthread_mutex_init(
			reinterpret_cast<pthread_mutex_t *>(imp_), // pthread_mutex_t *mutex
			0 );                                       // const pthread_mutexattr_t *attr
		if (err)
		{
			//char const * psz = 0;
			//switch (err)
			//{				Error code definitions from The Open Group - Single UNIX® Specification, Version 2
			//case EAGAIN:
			//	psz = "The system lacked the necessary resources (other than memory) to initialise another mutex.";
			//	break;
			//case ENOMEM:
			//	psz = "Insufficient memory exists to initialise the mutex.";
			//	break;
			//case EPERM:
			//	psz = "The caller does not have the privilege to perform the operation.";
			//	break;
			//case EBUSY:
			//	psz = "The implementation has detected an attempt to re-initialise the object referenced by mutex, "
			//		"a previously initialised, but not yet destroyed, mutex.";
			//	break;
			//case EINVAL:
			//	psz = "The value specified by attr is invalid.";
			//	break;
			//}
			throw 0;
		}

#elif 0
#elif 1
#	error ""
#endif
	}

	//-----------------------------------------------------------------------------------------------------------------|

	mutex::~mutex()
	{
#if IMPLEMENTATION_PTHREAD

		//	http://pubs.opengroup.org/onlinepubs/007908799/xsh/pthread_mutex_destroy.html
		int err = ::pthread_mutex_destroy(
			reinterpret_cast<pthread_mutex_t *>(imp_) ); // pthread_mutex_t *mutex
		if (err)
		{
			//char const * psz = 0;
			//switch (err)
			//{				Error code definitions from The Open Group - Single UNIX® Specification, Version 2
			//case EAGAIN:
			//	psz = "The system lacked the necessary resources (other than memory) to initialise another mutex.";
			//	break;
			//case ENOMEM:
			//	psz = "Insufficient memory exists to initialise the mutex.";
			//	break;
			//case EPERM:
			//	psz = "The caller does not have the privilege to perform the operation.";
			//	break;
			//case EBUSY:
			//	psz = "The implementation has detected an attempt to destroy the object referenced by mutex while it "
			//		"is locked or referenced (for example, while being used in a pthread_cond_wait() or "
			//		"pthread_cond_timedwait()) by another thread.";
			//	break;
			//case EINVAL:
			//	psz = "The value specified by mutex is invalid.";
			//	break;
			//}
			throw 0;
		}

#elif 0
#elif 1
#	error ""
#endif
	}

	//-----------------------------------------------------------------------------------------------------------------|

	mutex_lock mutex::lock()
	{
#if IMPLEMENTATION_PTHREAD

		//	http://pubs.opengroup.org/onlinepubs/007908799/xsh/pthread_mutex_lock.html
		int err = ::pthread_mutex_lock(
			reinterpret_cast<pthread_mutex_t *>(&imp_) ); // pthread_mutex_t *mutex
		if (err)
		{
			//char const * psz = 0;
			//switch (err)
			//{				Error code definitions from The Open Group - Single UNIX® Specification, Version 2
			//case EINVAL:
			//	psz = "The mutex was created with the protocol attribute having the value PTHREAD_PRIO_PROTECT "
			//		"and the calling thread's priority is higher than the mutex's current priority ceiling ";
			//		"or the value specified by mutex does not refer to an initialised mutex object.";
			//	break;
			//case EAGAIN:
			//	psz = "The mutex could not be acquired because the maximum number of recursive locks for mutex "
			//		"has been exceeded.";
			//	break;
			//case EDEADLK:
			//	psz = "The current thread already owns the mutex.";
			//	break;
			//}
			throw 0;
		}

#elif 0
#elif 1
#	error ""
#endif
		return mutex_lock(mutex_lock::private_construct_tag(), this);
	}

	//-----------------------------------------------------------------------------------------------------------------|

	optional<mutex_lock> mutex::try_lock()
	{
#if IMPLEMENTATION_PTHREAD

		//	http://pubs.opengroup.org/onlinepubs/007908799/xsh/pthread_mutex_lock.html
		int err = ::pthread_mutex_trylock(
			reinterpret_cast<pthread_mutex_t *>(&imp_) ); // pthread_mutex_t *mutex

		//char const * psz = 0;
		switch (err)
		{
		case 0:
			return mutex_lock(mutex_lock::private_construct_tag(), this);

		case EBUSY:
			//	psz = "The mutex could not be acquired because it was already locked.";
			return optional<mutex_lock>();

		default:

			//case EINVAL:
			//	psz = "The mutex was created with the protocol attribute having the value PTHREAD_PRIO_PROTECT "
			//		"and the calling thread's priority is higher than the mutex's current priority ceiling ";
			//		"or the value specified by mutex does not refer to an initialised mutex object.";
			//	break;

			//case EAGAIN:
			//	psz = "The mutex could not be acquired because the maximum number of recursive locks for mutex "
			//		"has been exceeded.";
			//	break;
			//}

			throw 0;
		}

#elif 0
#elif 1
#	error ""
#endif
		throw 0;
	}

} // qak ==============================================================================================================|
