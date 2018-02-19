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
//	mutex__test.cxx

#include "qak/mutex.hxx"

#include "qak/test_app_pre.hxx"
#include "qak/test_macros.hxx"

using qak::mutex;
using qak::mutex_lock;
using qak::optional;

namespace zzz { //=====================================================================================================|

	QAKtest(mutex_default_construct)
	{
		mutex mut;
	}

	QAKtest(mutex_lock)
	{
		mutex mut;
		mutex_lock lock(mut);
		QAK_verify( lock.is_locking(mut) );
	}

	QAKtest(mutex_try_lock)
	{
		mutex mut;

		qak::optional<mutex_lock> opt_lock1 = mut.try_lock();
		QAK_verify( opt_lock1 );
		QAK_verify( opt_lock1->is_locking(mut) );
		QAK_verify( mut.is_locked_by_this_thread() );

		opt_lock1.reset();

		QAK_verify( !mut.is_locked_by_this_thread() );

		//	Really we need another thread to test this.
		//qak::optional<mutex_lock> opt_lock2 = mut.try_lock();
		//QAK_verify( !opt_lock2 );
	}

	QAKtest(mutex_try_lock_2)
	{
		mutex mut;
		qak::optional<mutex_lock> opt_lock = mut.try_lock();
		QAK_verify( opt_lock );
		QAK_verify( opt_lock->is_locking(mut) );
	}

	QAKtest(mutex_non_recursively_acquireable)
	{
		mutex mut;

		mutex_lock lock = mut.lock();
		QAK_verify( lock.is_locking(mut) );
		QAK_verify( mut.is_locked_by_this_thread() );

		bool throwed = false;
		try
		{
			mut.try_lock();
		}
		catch (...) { throwed = true; }

		QAK_verify(throwed);
	}

} // namespace zzz ====================================================================================================|
#include "qak/test_app_post.hxx"
