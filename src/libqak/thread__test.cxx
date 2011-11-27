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
//	thread_test.cxx

#include "qak/thread.hxx"

#include "qak/atomic.hxx"
#include "qak/host_info.hxx"
#include "qak/stopwatch.hxx"

namespace zzz { //=====================================================================================================|

	void do_it()
	{
		{
			//	Test starting and joining a thread.

			qak::atomic<bool> b(false);

			qak::atomic<qak::thread_id> tid0(0);

			qak::thread_id tid1 = qak::start_thread(
				[&b, &tid0] // std::function<void()> thread_fn
				{
					qak::this_thread::yield();
					if (!( !b )) throw 0;
					qak::this_thread::yield();
					b = true;
					qak::this_thread::yield();
					tid0 = qak::this_thread::get_id();
					qak::this_thread::yield();
				});

			qak::join_thread(tid1);

			if (!( b )) throw 0;
			if (!( tid0 == tid1 )) throw 0;
		} {
			//	Test sleep_ns().

			qak::stopwatch sw;
			qak::this_thread::sleep_ns(1000*1000);
			if (!(  1000*1000 <= sw.elapsed_ns() )) throw 0;
		} {
			//	Test that we can cycle through affinity with all the cpus.

			qak::thread_id tid = qak::start_thread(
				[] {
					unsigned cnt = qak::host_info::cnt_cpus_available();
					for (unsigned cpu_ix = 0; cpu_ix < cnt*5; ++cpu_ix)
					{
						qak::this_thread::set_affinity(cpu_ix);
						qak::stopwatch sw;
						while (sw.elapsed_s() < 0.1/cnt/5); // busy
					}
				});

			qak::join_thread(tid);
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
