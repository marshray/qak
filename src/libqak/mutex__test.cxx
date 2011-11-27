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

#include "qak/fail.hxx"

using qak::throw_if;
using qak::throw_unless;

namespace zzz { //=====================================================================================================|

	void do_it()
	{
		{
			qak::mutex mut;
		} {
			qak::mutex mut;
			qak::mutex_lock lock(mut);
			throw_unless(  lock.is_locking(mut)  );
		} {
			qak::mutex mut;
			qak::mutex_lock lock = mut.lock();
			throw_unless(  lock.is_locking(mut)  );

			qak::optional<qak::mutex_lock> opt_lock = mut.try_lock();
			throw_unless(  !opt_lock  );
		} {
			qak::mutex mut;
			qak::optional<qak::mutex_lock> opt_lock = mut.try_lock();
			throw_unless(  opt_lock  );
			throw_unless(  opt_lock->is_locking(mut)  );
		}
	}

} // namespace zzz ====================================================================================================|

	int main(int, char * [])
	{
		int rc = 1;
		try
		{
			zzz::do_it();
			rc = 0;
		}
		catch (...) { rc |= 2; }

		return rc;
	}

//=====================================================================================================================|
