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
//	thread__test.cxx

#include "qak/thread.hxx"

#include "qak/atomic.hxx"
#include "qak/host_info.hxx"
#include "qak/stopwatch.hxx"

#include "qak/test_app_pre.hxx"
#include "qak/test_macros.hxx"

namespace zzz { //=====================================================================================================|

	QAKtest_anon()
	{
		//	Test starting and joining a thread.

		qak::atomic<bool> b(false);

		qak::atomic<qak::thread_id> tid0(0);

		qak::thread_id tid1 = qak::start_thread(
			[&b, &tid0] // std::function<void()> thread_fn
			{
				qak::this_thread::yield();
				QAK_verify( !b );
				qak::this_thread::yield();
				b = true;
				qak::this_thread::yield();
				tid0 = qak::this_thread::get_id();
				qak::this_thread::yield();
			});

		qak::join_thread(tid1);

		QAK_verify( b );
		QAK_verify( tid0 == tid1 );
	}

	QAKtest_anon()
	{
		//	Test sleep_ns().

		qak::stopwatch sw;
		qak::this_thread::sleep_ns(1000*1000);
		QAK_verify( 1000*1000 <= sw.elapsed_ns() );
	}

	QAKtest_anon()
	{
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

} // namespace zzz ====================================================================================================|
#include "qak/test_app_post.hxx"
