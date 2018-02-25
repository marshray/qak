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
//	now__test.cxx

#include "qak/now.hxx"

#include "qak/config.hxx"
#include "qak/thread.hxx"
#include "qak/stopwatch.hxx"
#include "qak/test_app_pre.hxx"
#include "qak/test_macros.hxx"

namespace zzz { //=====================================================================================================|

    QAKtest(wallclock_ns)
    {
        int64_t t_ns = read_time_source(qak::time_source::wallclock_ns);
        QAK_verify( 1 < t_ns && t_ns < int64_t(10)*1000*1000*1000 );
    }

    QAKtest(sleep_ns)
	{
        qak::stopwatch sw;
        qak::this_thread::sleep_ns(1000);
        double elapsed = sw.elapsed_s();
        QAK_verify( 900e-9 <= elapsed && elapsed < 1.0);
    }

	QAKtest(realtime_ns)
	{
		qak::this_thread::sleep_ns(1000);
		int64_t t_ns = read_time_source(qak::time_source::realtime_ns);
		QAK_verify( 1 < t_ns && t_ns < int64_t(10)*1000*1000*1000 );
	}

	QAKtest(cpu_thread_ns)
	{
		qak::this_thread::sleep_ns(1000);
		int64_t t0_ns = read_time_source(qak::time_source::cpu_thread_ns);
		bool advanced = false;
		for (unsigned n = 0; n < 1*1000*1000*1000; ++n)
		{
			int64_t t1_ns = read_time_source(qak::time_source::cpu_thread_ns);
			if (t0_ns < t1_ns)
			{
				advanced = true;
				break;
			}
		}
		QAK_verify( advanced );
	}

	QAKtest(cpu_cycles)
	{
		qak::this_thread::sleep_ns(1000);
		int64_t t0_cycles = read_time_source(qak::time_source::cpu_cycles);
		QAK_verify( t0_cycles );
		qak::this_thread::sleep_ns(1000);
		int64_t t1_cycles = read_time_source(qak::time_source::cpu_cycles);
		QAK_verify( t0_cycles != t1_cycles );
	}

} // namespace zzz ====================================================================================================|
#include "qak/test_app_post.hxx"
