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

namespace qak { //=====================================================================================================|

	struct mutex;
	struct mutex_lock;

	//=================================================================================================================|

	//	Basic nonrecursive mutex.
	//
	struct mutex
	{
		mutex();
		~mutex();

		//	Noncopyable, nonmoveable.
#if !QAK_COMPILER_FAILS_DELETED_MEMBERS // supports "= delete" syntax

		mutex(mutex const &) = delete;
		mutex(mutex &&) = delete;
		mutex & operator = (mutex const &) = delete;
		mutex & operator = (mutex &&) = delete;

#else // workaround for compilers that don't support "= delete" syntax

	private:
		mutex(mutex const &); // unimplemented
		mutex(mutex &&); // unimplemented
		mutex & operator = (mutex const &); // unimplemented
		mutex & operator = (mutex &&); // unimplemented
	public:

#endif // of workaround for compilers that don't support "= delete" syntax

		//	Blocks forever, or until the mutex is acquired, whichever comes first.
		//	Throws if the mutex is already locked by the current thread.
		mutex_lock lock();

		//	Detects if the mutex is already locked by the calling thread. Check
		//	this before attempting to lock the mutex in order to 
		bool is_locked_by_this_thread() const;

		//	Acquires the mutex iff it is currently free. Does not block.
		//	Throws if the mutex is already locked by the current thread.
		optional<mutex_lock> try_lock();

	private:
		friend struct mutex_lock;

#if QAK_THREAD_PTHREAD

		unsigned long long imp_[
				  1 // atomic owning thread ID

				//	Actual pthread mutex is 24 or 40 bytes
				+ (24 + 4*(QAK_pointer_bits - 32)/8 + sizeof(unsigned long long) - 1)/sizeof(unsigned long long)
			];

#elif QAK_THREAD_WIN32

		unsigned long long imp_[
				  1 // atomic owning thread ID

				//	Actual Win32 CRITICAL_SECTION is 24 or 40 bytes
				+ (24 + 4*(QAK_pointer_bits - 32)/8 + sizeof(unsigned long long) - 1)/sizeof(unsigned long long)
			];

#else
#	error "port me"
#endif
	};

	//-----------------------------------------------------------------------------------------------------------------|

	//	With a single exception*, the existence of a mutex_lock means that a mutex is actually locked.
	//
	//	* The source object after a std::move operation, which is not necessarily a defined state anyway.
	//
	struct mutex_lock
	{
		//	Blocks forever, or until the mutex is acquired, whichever comes first.
		explicit mutex_lock(mutex & m);

		//	Noncopyable.

#if !QAK_COMPILER_FAILS_DELETED_MEMBERS // supports "= delete" syntax

		mutex_lock() = delete;
		mutex_lock(mutex_lock const &) = delete;
		mutex_lock & operator = (mutex_lock const &) = delete;
		mutex_lock & operator = (mutex_lock &&) = delete;

#else // workaround for compilers that don't support "= delete" syntax

	private:
		mutex_lock(); // unimplemented
		mutex_lock(mutex_lock const &); // unimplemented
		mutex_lock & operator = (mutex_lock const &); // unimplemented
		mutex_lock & operator = (mutex_lock &&); // unimplemented
	public:

#endif // of workaround for compilers that don't support "= delete" syntax

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
