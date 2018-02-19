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
//#include "qak/mutex.hxx"
//
//	A minimal mutex facility.

#ifndef qak_mutex_hxx_INCLUDED_
#define qak_mutex_hxx_INCLUDED_

#include "qak/config.hxx"
#include "qak/optional.hxx"
#include "qak/imp/pthread.hxx"

namespace qak { //=====================================================================================================|

	struct mutex;
	struct mutex_lock;

	//=================================================================================================================|

	//	Basic nonrecursive mutex.
	//
	struct mutex
	{
		typedef mutex_lock lock_type;

		mutex();
		~mutex();

		//	Noncopyable, nonmoveable.

		mutex(mutex const &) = delete;
		mutex(mutex &&) = delete;
		mutex & operator = (mutex const &) = delete;
		mutex & operator = (mutex &&) = delete;

		//	Blocks forever, or until the mutex is acquired, whichever comes first.
		//	Throws if the mutex is already locked by the current thread.
		lock_type lock();

		//	Detects if the mutex is already locked by the calling thread. Check
		//	this before attempting to lock the mutex in order to avoid an exception.
		bool is_locked_by_this_thread() const;

		//	Acquires the mutex iff it is currently free. Does not block.
		//	Throws if the mutex is already locked by the current thread.
		optional<lock_type> try_lock();

	private:
		friend struct mutex_lock;

#if QAK_THREAD_PTHREAD

		typedef unsigned long long imp_elem_t;
		typedef imp_elem_t imp_t[
				  1 // atomic owning thread ID

				//	Actual pthread mutex is 24 or 40 bytes
				+ (24 + 4*(QAK_pointer_bits - 32)/8 + sizeof(unsigned long long) - 1)/sizeof(unsigned long long)
			];

#elif QAK_THREAD_WIN32

		typedef unsigned long long imp_elem_t;
		typedef imp_elem_t imp_t[
				  1 // atomic owning thread ID

				//	Actual Win32 CRITICAL_SECTION is 24 or 40 bytes
				+ (24 + 4*(QAK_pointer_bits - 32)/8 + sizeof(unsigned long long) - 1)/sizeof(unsigned long long)
			];

#else
#	error "port me"
#endif

		imp_t imp_;
	};

	//-----------------------------------------------------------------------------------------------------------------|

	//	With a single exception*, the existence of a mutex_lock means that a mutex is actually locked.
	//
	//	* The source object after a std::move operation, which is not necessarily a defined state anyway.
	//
	// If you absolutely must have a lock that you can release before the end of its natural scope, you can do:
	//		std::shared_ptr<qak::mutex::lock_type> sp_lock(
	//				new qak::mutex::lock_type(std::move(allow_thread_exit.lock())) );
	//
	struct mutex_lock
	{
		//	Blocks forever, or until the mutex is acquired, whichever comes first.
		explicit mutex_lock(mutex & m);

		//	Noncopyable.

		mutex_lock() = delete;
		mutex_lock(mutex_lock const &) = delete;
		mutex_lock & operator = (mutex_lock const &) = delete;
		mutex_lock & operator = (mutex_lock &&) = delete;

		//	Move-constructable (but be careful).
		mutex_lock(mutex_lock &&);

		//	Releases the lock held on the mutex.
		~mutex_lock();

		//	Returns true iff the caller identifies the mutex on which this object holds a lock.
		bool is_locking(mutex const & m) const;

	private:
		friend struct mutex;

		struct private_construct_tag { };
		mutex_lock(private_construct_tag const &, mutex * pm);

		mutex * p_m_;
	};

} // namespace qak ====================================================================================================|
#endif // ndef qak_mutex_hxx_INCLUDED_
