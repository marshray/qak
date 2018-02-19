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

#include "qak/alignof.hxx"
#include "qak/atomic.hxx"
#include "qak/config.hxx"
#include "qak/fail.hxx"
#include "qak/thread.hxx" // thread_id_t

#include <cstdint>
#include <cassert>

#include <iostream>//?
#include <iomanip>//?

#if QAK_THREAD_PTHREAD
	// Error code definitions from The Open Group - Single UNIX Specification, Version 2
#	include <errno.h> // pthread error codes
#	include <pthread.h>
#elif QAK_THREAD_WIN32
#	include "../platforms/win32/win32_lite.hxx"
#endif

namespace qak { //=====================================================================================================|

	typedef qak::atomic<thread_id_t> atomic_thread_id_t;

#	define OWNING_THREAD_ID(p)       (*reinterpret_cast<atomic_thread_id_t *      >(&p->imp_[0]))
#	define OWNING_THREAD_ID_const(p) (*reinterpret_cast<atomic_thread_id_t const *>(&p->imp_[0]))

#if QAK_THREAD_PTHREAD

#	define MUTEX(p) (*reinterpret_cast<pthread_mutex_t *>(&p->imp_[1]))

#elif QAK_THREAD_WIN32

#	define CRITSEC(p) (*reinterpret_cast<win32::CRITICAL_SECTION *>(&p->imp_[1]))

#endif // of elif QAK_THREAD_WIN32

	//-----------------------------------------------------------------------------------------------------------------|

	mutex::mutex() // : imp_ not initialized
	{
		static_assert(sizeof(atomic_thread_id_t) <= sizeof(imp_elem_t), "increase size of imp_t");
		static_assert(QAK_alignof_t(atomic_thread_id_t) <= QAK_alignof_x(imp_elem_t), "increase align of imp_");

#if QAK_THREAD_PTHREAD

		new (&OWNING_THREAD_ID(this)) atomic_thread_id_t(invalid_thread_id_value);

		static_assert(sizeof(pthread_mutex_t) <= sizeof(imp_t) - sizeof(imp_elem_t), "increase size of imp_t");
		static_assert(QAK_alignof_t(pthread_mutex_t) <= QAK_alignof_t(imp_elem_t), "increase align of imp_elem_t");

		//	http://pubs.opengroup.org/onlinepubs/007908799/xsh/pthread_mutex_init.html
		int err = ::pthread_mutex_init(
			& MUTEX(this), // pthread_mutex_t *mutex
			0 );           // const pthread_mutexattr_t *attr
		if (err)
		{
			//char const * psz = 0;
			//switch (err)
			//{				Error code definitions from The Open Group - Single UNIX Specification, Version 2
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
			fail(); // internal error
		}

#elif QAK_THREAD_WIN32

		new (&OWNING_THREAD_ID(this)) atomic_thread_id_t(win32::INVALID_THREAD_ID);

		static_assert(sizeof(win32::CRITICAL_SECTION) <= sizeof(imp_t) - sizeof(imp_elem_t), "increase size of imp_t");
		static_assert(QAK_alignof_t(win32::CRITICAL_SECTION) <= QAK_alignof_t(imp_elem_t), "increase align of imp_elem_t");

		win32::InitializeCriticalSection(&CRITSEC(this));

#else
#	pragma message "error: port me"
#endif
	}

	//-----------------------------------------------------------------------------------------------------------------|

	mutex::~mutex()
	{
#if QAK_THREAD_PTHREAD
//std::cerr << "pthread_mutex_destroy\n";//?

		//	http://pubs.opengroup.org/onlinepubs/007908799/xsh/pthread_mutex_destroy.html
		int err = ::pthread_mutex_destroy(
			&MUTEX(this) ); // pthread_mutex_t *mutex
		if (err)
		{
			char const * psz = "Unknown mutex_destroy error.";
			switch (err)
			{				// Error code definitions from The Open Group - Single UNIX Specification, Version 2
			case EAGAIN:
				psz = "The system lacked the necessary resources (other than memory) to initialise another mutex.";
				break;
			case ENOMEM:
				psz = "Insufficient memory exists to initialise the mutex.";
				break;
			case EPERM:
				psz = "The caller does not have the privilege to perform the operation.";
				break;
			case EBUSY:
				psz = "The implementation has detected an attempt to destroy the object referenced by mutex while it "
					"is locked or referenced (for example, while being used in a pthread_cond_wait() or "
					"pthread_cond_timedwait()) by another thread.";
				break;
			case EINVAL:
				psz = "The value specified by mutex is invalid.";
				break;
			}
std::cerr << psz << "\n";//?
			assert(!psz);
			//? log this message?
		}

#elif QAK_THREAD_WIN32

		//	There is no conceivable circumstance in which it's valid to delete the critsec while any mutex_lock owns it.
		assert(OWNING_THREAD_ID_const(this) == win32::INVALID_THREAD_ID);

		OWNING_THREAD_ID(this).~atomic_thread_id_t();

		win32::DeleteCriticalSection(&CRITSEC(this));

#else
#	pragma message "error: port me"
#endif
	}

	//-----------------------------------------------------------------------------------------------------------------|

	mutex::lock_type mutex::lock()
	{
#if QAK_THREAD_PTHREAD

		//	http://pubs.opengroup.org/onlinepubs/007908799/xsh/pthread_mutex_lock.html
		int err = ::pthread_mutex_lock(
			&MUTEX(this) ); // pthread_mutex_t *mutex
		if (err)
		{
			char const * psz = "Unknown mutex_lock error.";
			switch (err)
			{				// Error code definitions from The Open Group - Single UNIX® Specification, Version 2
			case EINVAL:
				psz = "The mutex was created with the protocol attribute having the value PTHREAD_PRIO_PROTECT "
					"and the calling thread's priority is higher than the mutex's current priority ceiling "
					"or the value specified by mutex does not refer to an initialised mutex object.";
				break;
			case EAGAIN:
				psz = "The mutex could not be acquired because the maximum number of recursive locks for mutex "
					"has been exceeded.";
				break;
			case EDEADLK:
				psz = "The current thread already owns the mutex.";
				break;
			}
std::cerr << psz << "\n";//?
			assert(!psz);
			fail(); // internal error
		}

		assert(::pthread_equal(OWNING_THREAD_ID_const(this), invalid_thread_id_value));

		OWNING_THREAD_ID(this) = ::pthread_self();

		assert(!::pthread_equal(OWNING_THREAD_ID_const(this), invalid_thread_id_value));

#elif QAK_THREAD_WIN32

		win32::EnterCriticalSection(&CRITSEC(this));

		if (OWNING_THREAD_ID_const(this) == win32::GetCurrentThreadId())
		{
			win32::LeaveCriticalSection(&CRITSEC(this));
			fail(); //	Already owns the mutex.
		}

		assert(OWNING_THREAD_ID_const(this) == win32::INVALID_THREAD_ID);

		OWNING_THREAD_ID(this) = win32::GetCurrentThreadId();

#else
#	pragma message "error: port me"
#endif

		return mutex_lock(mutex_lock::private_construct_tag(), this);
	}

	//-----------------------------------------------------------------------------------------------------------------|

	bool mutex::is_locked_by_this_thread() const
	{
#if QAK_THREAD_PTHREAD

		return !! ::pthread_equal(OWNING_THREAD_ID_const(this), ::pthread_self());

#elif QAK_THREAD_WIN32

		return OWNING_THREAD_ID_const(this) == win32::GetCurrentThreadId();

#else
#	error "port me"
#endif
	}

	//-----------------------------------------------------------------------------------------------------------------|

	optional<mutex::lock_type> mutex::try_lock()
	{
		if (this->is_locked_by_this_thread())
			fail(); //	Already owns the mutex.

#if QAK_THREAD_PTHREAD

		//	http://pubs.opengroup.org/onlinepubs/007908799/xsh/pthread_mutex_lock.html
		int err = ::pthread_mutex_trylock(
			&MUTEX(this) ); // pthread_mutex_t *mutex

		//char const * psz = 0;
		switch (err)
		{
		case 0:
			break; // returns

		case EBUSY:
			//	psz = "The mutex could not be acquired because it was already locked.";
			return optional<mutex_lock>();

		//case EINVAL:
		//	psz = "The mutex was created with the protocol attribute having the value PTHREAD_PRIO_PROTECT "
		//		"and the calling thread's priority is higher than the mutex's current priority ceiling ";
		//		"or the value specified by mutex does not refer to an initialised mutex object.";
		//	break;

		//case EAGAIN:
		//	psz = "The mutex could not be acquired because the maximum number of recursive locks for mutex "
		//		"has been exceeded.";
		//	break;

		default:
			throw 0;
		}

		assert(OWNING_THREAD_ID_const(this) == invalid_thread_id_value);

		OWNING_THREAD_ID(this) = ::pthread_self();

#elif QAK_THREAD_WIN32

		win32::BOOL entered = win32::TryEnterCriticalSection(&CRITSEC(this));

		if (!entered)
			return optional<mutex_lock>();

		assert(OWNING_THREAD_ID_const(this) == win32::INVALID_THREAD_ID);

		OWNING_THREAD_ID(this) = win32::GetCurrentThreadId();

#else
#	error "port me"
#endif

		return mutex_lock(mutex_lock::private_construct_tag(), this);
	}

	//-----------------------------------------------------------------------------------------------------------------|

	mutex_lock::mutex_lock(
		mutex & m
	) :
		p_m_(0)
	{
		mutex_lock ml = m.lock();
		std::swap(ml.p_m_, p_m_);

		//	Verify we have a lock on mutex *p_m_.
#if QAK_THREAD_PTHREAD

		assert(::pthread_equal(OWNING_THREAD_ID_const(p_m_), ::pthread_self()));

#elif QAK_THREAD_WIN32

		assert(p_m_->is_locked_by_this_thread());

#endif // of elif QAK_THREAD_WIN32
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
#if QAK_THREAD_PTHREAD

			//	Verify we have a lock on mutex *p_m_.
			assert(::pthread_equal(OWNING_THREAD_ID_const(p_m_), ::pthread_self()));

			OWNING_THREAD_ID(p_m_) = invalid_thread_id_value;

			int err = ::pthread_mutex_unlock(
				&MUTEX(p_m_) ); // pthread_mutex_t *mutex
			assert(!err);
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
				fail(); // internal error
			}

#elif QAK_THREAD_WIN32

		assert(OWNING_THREAD_ID_const(p_m_) == win32::GetCurrentThreadId());

		OWNING_THREAD_ID(p_m_) = win32::INVALID_THREAD_ID;

		win32::LeaveCriticalSection(&CRITSEC(p_m_));

#else
#	error "port me"
#endif
		}
	}

	//-----------------------------------------------------------------------------------------------------------------|

	mutex_lock::mutex_lock(mutex_lock && that) : p_m_(that.p_m_)
	{
		that.p_m_ = 0;

		//	Verify we have a lock on mutex *p_m_.
		assert(p_m_);

#if QAK_THREAD_PTHREAD

		thread_id_t tid_self = ::pthread_self();
		thread_id_t tid_owning = OWNING_THREAD_ID_const(p_m_);

		assert(::pthread_equal(OWNING_THREAD_ID_const(p_m_), ::pthread_self()));

#elif QAK_THREAD_WIN32

		assert(OWNING_THREAD_ID_const(p_m_) == win32::GetCurrentThreadId());

#else
#	error "port me"
#endif
	}

	//-----------------------------------------------------------------------------------------------------------------|

	bool mutex_lock::is_locking(mutex const & m) const
	{
		return &m == p_m_;
	}

} // qak ==============================================================================================================|
